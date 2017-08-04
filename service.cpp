/*
 * Copyright (c) 2017 Fraunhofer FOKUS
 *
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include <sys/signal.h>
#include <sys/time.h>

#include <fstream>
#include <iostream>
#include <map>
#include <thread>
#include <string>
#include <vector>

#include "cdmi.h"

// shared mem und semaphores stuff
#include "libs/shmemsem/shmemsem_helper.h"

extern "C" {
#include "opencdm_xdr.h"
#include "opencdm_callback.h"
}

using namespace CDMi;
using namespace std;

IMediaKeys *g_pMediaKeys = NULL;
map<wstring, IMediaKeySession*> g_mediaKeySessions;

// TODO(fhg): get rid of more globals
vector<IMediaEngineSession *> g_mediaEngineSessions;
char g_hostname[256];
u_long g_pnum;  // program number for callback routine

void doCallback(int, string, int, const wchar_t*);


class CCallback : public IMediaKeySessionCallback {
 public:
  CCallback(const IMediaKeySession *fp_mediaKeySession) {
    m_mediaKeySession = const_cast<IMediaKeySession *>(fp_mediaKeySession);
  }

  virtual ~CCallback(void) {}

  virtual void OnKeyMessage(
      const uint8_t *pbKeyMessage,
      uint32_t cbKeyMessage,
      char *f_pszUrl) {
    uint8_t *pbChallenge = NULL;
    uint32_t cbChallenge = 0;
    string message;

    cout << "OnKeyMessage: Key message received." << endl;

    message = string(f_pszUrl) + "#SPLIT#" +
        string(reinterpret_cast<char*>(pbChallenge), cbChallenge);

    cout << "key message: [" << message << "]" << endl;

    doCallback(ON_MESSAGE, message.c_str(), 0, m_mediaKeySession->GetSessionId());
  }

  virtual void OnKeyReady(void) {
    cout << "OnKeyReady: Key is ready." << endl;
    doCallback(ON_READY, "", 0, m_mediaKeySession->GetSessionId());
  }

  virtual void OnKeyError(CDMi_RESULT error) {
    cout << "Key error is detected: " << error << endl;
    doCallback(ON_ERROR, "", error, m_mediaKeySession->GetSessionId());
  }

 private:
  IMediaKeySession *m_mediaKeySession;
};

void wcstoui16tp(uint16_t *dst, const wchar_t *sid) {
  for (int i = 0; i < wcslen(sid); i++) {
    dst[i] = sid[i];
  }
}

wstring ui16tptowcs(uint16_t* src, uint16_t len) {
  char *buffer = reinterpret_cast<char*>(malloc(sizeof(char) * (len + 1)));
  std::wstring wc(len, L'#');

  for (int i = 0; i < len; i++) {
    buffer[i] = src[i];
  }
  buffer[len] = '\0';

  mbstowcs(&wc[0], buffer, len + 1);
  free(buffer);
  return wc;
}


rpc_response_generic* rpc_open_cdm_is_type_supported_1_svc(
    rpc_request_is_type_supported *type, struct svc_req *) {
  static CDMi_RESULT cr = CDMi_SUCCESS;
  rpc_response_generic *response =
      reinterpret_cast<rpc_response_generic*>(
      malloc(sizeof(rpc_response_generic)));

  cout << "#rpc_open_cdm_is_type_supported_1_svc: "
       << type->key_system.key_system_val << endl;
  if (g_pMediaKeys) {
    cr = g_pMediaKeys->IsTypeSupported(
        reinterpret_cast<wchar_t *>(type->key_system.key_system_val),
        reinterpret_cast<wchar_t *>(type->mime_type.mime_type_val));
  } else {
    cr = CDMi_S_FALSE;
  }

  response->platform_val = cr;
  return response;
}

rpc_response_generic* rpc_open_cdm_mediakeys_1_svc(
  rpc_request_mediakeys *keysystem, struct svc_req *) {
  static CDMi_RESULT cr = CDMi_S_FALSE;
  rpc_response_generic *response =
      reinterpret_cast<rpc_response_generic*>(
      malloc(sizeof(rpc_response_generic)));

  cout << "#rpc_open_cdm_mediakeys_1_svc: "
       << keysystem->key_system.key_system_val << endl;
  cr = CreateMediaKeys(&g_pMediaKeys);

  response->platform_val = cr;
  return response;
}

rpc_response_create_session* rpc_open_cdm_mediakeys_create_session_1_svc(
  rpc_request_create_session *sessionmessage, struct svc_req *) {
  static CDMi_RESULT cr = CDMi_S_FALSE;
  rpc_response_create_session *response =
      reinterpret_cast<rpc_response_create_session*>(
      malloc(sizeof(rpc_response_create_session)));
  IMediaKeySessionCallback *callback = NULL;
  uint16_t *dst;

  // callback_info for info on how to rpc callback into browser
  cout << "#open_cdm_mediakeys_create_session_1_svc: prog num: "
       <<  sessionmessage->callback_info.prog_num << endl;
  g_pnum = sessionmessage->callback_info.prog_num;

  cout << "#open_cdm_mediakeys_create_session_1_svc init_data_val: "
       << &sessionmessage->init_data_type.init_data_type_val << endl;
  if (g_pMediaKeys) {
    IMediaKeySession *p_mediaKeySession =
        reinterpret_cast<IMediaKeySession*>(malloc(sizeof(IMediaKeySession)));
    cr = g_pMediaKeys->CreateMediaKeySession(
        sessionmessage->init_data_type.init_data_type_val,
        sessionmessage->init_data.init_data_val,
        sessionmessage->init_data.init_data_len,
        NULL,
        0,
        &p_mediaKeySession);

    if (cr == CDMi_SUCCESS) {
      const wchar_t *sid = p_mediaKeySession->GetSessionId();
      g_mediaKeySessions[sid] = p_mediaKeySession;

      dst = reinterpret_cast<uint16_t*>(malloc(sizeof(uint16_t) * wcslen(sid)));
      wcstoui16tp(dst, sid);

      response->session_id.session_id_val = dst;
      response->session_id.session_id_len = wcslen(sid);

      cout << "#open_cdm_mediakeys_create_session_1_svc: creating new CCallbback" << endl;
      callback = new CCallback(p_mediaKeySession);

      // generates challenge
      p_mediaKeySession->Run(callback);
    }
  }

  response->platform_val = cr;
  return response;
}

rpc_response_generic* rpc_open_cdm_mediakeys_load_session_1_svc(
  rpc_request_load_session *params, struct svc_req *) {
  return NULL;  // TODO(fhg): NYI
}

rpc_response_generic* rpc_open_cdm_mediakeysession_update_1_svc(
  rpc_request_session_update *params, struct svc_req *) {
  static CDMi_RESULT cr = CDMi_SUCCESS;
  rpc_response_generic *response =
      reinterpret_cast<rpc_response_generic*>(
      malloc(sizeof(rpc_response_generic)));
  IMediaKeySession *p_mediaKeySession;

  wstring sid = ui16tptowcs(params->session_id.session_id_val,
      params->session_id.session_id_len);
  p_mediaKeySession = g_mediaKeySessions[sid];

  if (p_mediaKeySession) {
    wcout << "update for session id: " << sid << endl;
    cout << "key: [" << params->key.key_val << "]" << endl;
    p_mediaKeySession->Update(params->key.key_val,
        params->key.key_len);
    cr = CDMi_SUCCESS;
  } else {
    wcout << "no session found for session id: " << sid << endl;
    cr = CDMi_S_FALSE;
  }

  response->platform_val = cr;
  return response;
}

rpc_response_generic* rpc_open_cdm_mediakeysession_release_1_svc(
  rpc_request_session_release *params, struct svc_req *) {
  static CDMi_RESULT cr = CDMi_SUCCESS;
  rpc_response_generic *response =
      reinterpret_cast<rpc_response_generic*>(
      malloc(sizeof(rpc_response_generic)));
  IMediaKeySession *p_mediaKeySession;

  cout << "#open_cdm_mediakeysession_release_1_svc " << endl;

  wstring sid = ui16tptowcs(params->session_id.session_id_val,
      params->session_id.session_id_len);
  p_mediaKeySession = g_mediaKeySessions[sid];

  if (p_mediaKeySession) {
    p_mediaKeySession->Close();
    g_mediaKeySessions.erase(sid);
    free(p_mediaKeySession);
    cr = CDMi_SUCCESS;
  } else {
    cr = CDMi_S_FALSE;
  }

  response->platform_val = cr;
  return response;
}

void decryptShmem(int idxMES, int idXchngSem, int idXchngShMem) {
  shmem_info *mesShmem;
  IMediaEngineSession *pMediaEngineSession = NULL;
  mesShmem = (shmem_info *) MapSharedMemory(idXchngShMem);

  cout << "#decryptShmem: " << idxMES << endl;
  cout << "#decryptShmem: " << idXchngSem << endl;
  cout << "#decryptShmem: " << idXchngShMem << endl;

  for (;;) {
    /* ***** BENCHMARK begin ***** */
    timespec ts_bm_decrypt_start, ts_bm_decrypt_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_bm_decrypt_start);
    /* ***** BENCHMARK end ***** */

    CDMi_RESULT cr = CDMi_SUCCESS;
    if (g_mediaEngineSessions.size() -1 < idxMES) {
      cout << "decryptShmem: invalid media engine session idx: "
           << idxMES << endl;
      cr = CDMi_S_FALSE;
      return;
    }

    pMediaEngineSession = g_mediaEngineSessions.at(idxMES);

    if (pMediaEngineSession == NULL) {
      cout << "decryptShmem: no valid media engine session found" << endl;
      cr = CDMi_S_FALSE;
      return;
    } else {
      /*
       * TODO: (init, on create mes)
       *  1. transfer id of static info shmem (from client to cdmi)
       *  2. associate with mes
       * 
       * TODO:
       *  1. get both shmems for corresponding media engine
       *  2. wait for access (lock)
       *  3. get size and shmem id from static shmem
       *  4. get dynamic shmem with sampledata
       *  5. decrypt inplace
       *  6. unlock both shmem
       * 
       *  HOWTO: reach end of loop, signaling end of segment?
       */

      // lock own semaphore to get data
      LockSemaphore(idXchngSem, SEM_XCHNG_DECRYPT);

      if (mesShmem->idIvShMem == 0
          && mesShmem->idSampleShMem == 0
          && mesShmem->ivSize == 0
          && mesShmem->sampleSize == 0) {
        DetachExistingSharedMemory(mesShmem);

        UnlockSemaphore(idXchngSem, SEM_XCHNG_PULL);
        break;
      }

      uint8_t *mem_iv = (uint8_t *) MapSharedMemory(mesShmem->idIvShMem);
      uint8_t *mem_sample = (uint8_t *) MapSharedMemory(mesShmem->idSampleShMem);

      uint32_t *empty = new uint32_t[0];
      cr = pMediaEngineSession->Decrypt(mem_iv,
          mesShmem->ivSize,
          mem_sample,
          mesShmem->sampleSize,
          empty,
          0);

      // detach all shared memories!
      DetachExistingSharedMemory(mem_iv);
      DetachExistingSharedMemory(mem_sample);

      // unlock that clnt knows about finished decryption
      UnlockSemaphore(idXchngSem, SEM_XCHNG_PULL);
    }
  }
}

rpc_response_generic* rpc_open_cdm_mediaengine_1_svc(
  rpc_request_mediaengine_data *params, struct svc_req *) {
  static CDMi_RESULT cr = CDMi_S_FALSE;
  rpc_response_generic *response =
      reinterpret_cast<rpc_response_generic*>(
      malloc(sizeof(rpc_response_generic)));
  IMediaKeySession *p_mediaKeySession;
  IMediaEngineSession *pMediaEngineSession = NULL;

  cout << "#cdm_mediaenginesession_rpc_1_svc: "
      << params->id_exchange_shmem << " "
      << params->id_exchange_sem << endl;

  wstring sid = ui16tptowcs(params->session_id.session_id_val,
      params->session_id.session_id_len);
  p_mediaKeySession = g_mediaKeySessions[sid];

  cr = CreateMediaEngineSession(p_mediaKeySession,
      NULL,
      0,
      NULL,
      0,
      &pMediaEngineSession);

  if (cr == CDMi_SUCCESS) {
    g_mediaEngineSessions.push_back(pMediaEngineSession);
    thread t(decryptShmem,
        g_mediaEngineSessions.size() - 1,
        params->id_exchange_sem,
        params->id_exchange_shmem);
    t.detach();
  } else {
    cout << "MediaEngineSession create failed!" << endl;
  }

  response->platform_val = cr;
  return response;
}

void doCallback(
    int eventType,
    string message = "",
    int error = 0,
    const wchar_t *sid = NULL) {
  cout << "#doCallback" << endl;
  CLIENT *clnt;

  gethostname(g_hostname, sizeof(g_hostname));

  cout << "#doCallback: eventType: " <<  eventType << endl;
  cout << "#doCallback: hostname: " << g_hostname << endl;
  cout << "#doCallback: prog num: " <<  g_pnum << endl;
  if ((clnt = clnt_create(g_hostname, g_pnum, 1, "tcp")) == NULL) {
    cerr << "service: doCallback: clnt_create" << endl;
    clnt_pcreateerror(g_hostname);
    exit(2);
  }

  const char *temp_message = message.c_str();
  char ** msg = (char**) &temp_message;

  int * argp = reinterpret_cast<int*>(&error);
  uint16_t *dst =
      reinterpret_cast<uint16_t*>(malloc(sizeof(uint16_t) * wcslen(sid)));
  wcstoui16tp(dst, sid);

  switch (eventType) {
    case ON_MESSAGE:
      rpc_cb_message km;
      km.session_id.session_id_len = wcslen(sid);
      km.session_id.session_id_val = dst;
      km.destination_url = const_cast<char*>(temp_message);
      km.message = const_cast<char*>(temp_message);
      on_message_1(&km, clnt);
      break;

    case ON_READY:
      rpc_cb_ready kr;
      kr.session_id.session_id_len = wcslen(sid);
      kr.session_id.session_id_val = dst;
      on_ready_1(&kr, clnt);
      break;

    case ON_ERROR:
      rpc_cb_error ke;
      ke.session_id.session_id_len = wcslen(sid);
      ke.session_id.session_id_val = dst;
      on_error_1(&ke, clnt);
      break;

    default:
      cerr << "doCallback: unknown eventType" << endl;
    }
    free(dst);
}
