/*
 * Copyright 2014 Fraunhofer FOKUS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <pthread.h>

#include <iostream>

#include "imp.h"

#define DESTINATION_URL_PLACEHOLDER "http://no-valid-license-server"
#define NYI_KEYSYSTEM L"keysystem-placeholder"

using namespace std;

namespace CDMi {

CMediaKeySession::CMediaKeySession(const wchar_t *sessionId) {
  m_sessionId = sessionId;
  wcout << "creating mediakeysession with id: " << m_sessionId << endl;
}

CMediaKeySession::~CMediaKeySession(void) {}

void CMediaKeySession::Run(
    const IMediaKeySessionCallback *f_piMediaKeySessionCallback) {
  int ret;
  pthread_t thread;

  cout << "#mediakeysession.Run" << endl;

  if (f_piMediaKeySessionCallback != NULL) {
    m_piCallback = const_cast<IMediaKeySessionCallback *>(f_piMediaKeySessionCallback);

    ret = pthread_create(&thread, NULL, CMediaKeySession::_CallRunThread, this);
    if (ret == 0) {
      pthread_detach(thread);
    } else {
      cout << "#mediakeysession.Run: err: could not create thread" << endl;
      return;
    }
  } else {
    cout << "#mediakeysession.Run: err: MediaKeySessionCallback NULL?" << endl;
  }
}

void* CMediaKeySession::_CallRunThread(void *arg) {
  return ((CMediaKeySession*)arg)->RunThread(1);
}

void* CMediaKeySession::_CallRunThread2(void *arg) {
  return ((CMediaKeySession*)arg)->RunThread(2);
}

void* CMediaKeySession::RunThread(int i) {
  cout << "#mediakeysession._RunThread" << endl;

  if (i == 1) {
    m_piCallback->OnKeyMessage(NULL, 0, const_cast<char*>(DESTINATION_URL_PLACEHOLDER));
  } else {
    m_piCallback->OnKeyReady();
  }
}

void CMediaKeySession::Update(
    const uint8_t *f_pbKeyMessageResponse,
    uint32_t f_cbKeyMessageResponse) {
  int ret;
  pthread_t thread;

  cout << "#mediakeysession.Run" << endl;

  ret = pthread_create(&thread, NULL, CMediaKeySession::_CallRunThread2, this);
  if (ret == 0) {
    pthread_detach(thread);
  } else {
    cout << "#mediakeysession.Run: err: could not create thread" << endl;
    return;
  }
}

void CMediaKeySession::Close(void) {}

const wchar_t *CMediaKeySession::GetSessionId(void) const {
  return m_sessionId;
}

const wchar_t *CMediaKeySession::GetKeySystem(void) const {
  // TODO(fhg):
  return NYI_KEYSYSTEM;
}

CDMi_RESULT CMediaKeySession::Init(
    const uint8_t *f_pbInitData,
    uint32_t f_cbInitData,
    const uint8_t *f_pbCDMData,
    uint32_t f_cbCDMData) {
  return CDMi_SUCCESS;
}

// Decrypt is not an IMediaKeySession interface method therefore it can only be
// accessed from code that has internal knowledge of CMediaKeySession.
CDMi_RESULT CMediaKeySession::Decrypt(
    const uint8_t *f_pbSessionKey,
    uint32_t f_cbSessionKey,
    const uint8_t *f_pbIV,
    uint32_t f_cbIV,
    uint8_t *f_pbData,
    uint32_t f_cbData,
    const uint32_t *f_pdwSubSampleMapping,
    uint32_t f_cdwSubSampleMapping) {
  cout << "#CMediaKeySession::Decrypt" << endl;
  return CDMi_SUCCESS;
}

}  // namespace CDMi
