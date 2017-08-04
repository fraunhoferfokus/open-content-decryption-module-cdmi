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

#include <iostream>

#include "imp.h"

using namespace std;

namespace CDMi {

CMediaEngineSession::CMediaEngineSession(void) :
    m_pbSessionKey(NULL),
    m_cbSessionKey(0),
    m_pMediaKeySession(NULL) {}

CMediaEngineSession::~CMediaEngineSession(void) {}

CDMi_RESULT CMediaEngineSession::Decrypt(
    const uint8_t *f_pbIV,
    uint32_t f_cbIV,
    uint8_t *f_pbData,
    uint32_t f_cbData,
    const uint32_t *f_pdwSubSampleMapping,
    uint32_t f_cdwSubSampleMapping) {
  cout << "#CMediaEngineSession::Decrypt" << endl;

  CMediaKeySession *pMediaKeySession =
      static_cast<CMediaKeySession *>(m_pMediaKeySession);

  pMediaKeySession->Decrypt(m_pbSessionKey,
      m_cbSessionKey,
      f_pbIV,
      f_cbIV,
      f_pbData,
      f_cbData,
      f_pdwSubSampleMapping,
      f_cdwSubSampleMapping);

  return CDMi_SUCCESS;
}

CDMi_RESULT CMediaEngineSession::Init(
    const IMediaKeySession *f_piMediaKeySession,
    const uint8_t *f_pbSessionKey,
    uint32_t f_cbSessionKey) {
  m_pMediaKeySession = const_cast<IMediaKeySession *>(f_piMediaKeySession);

  return CDMi_SUCCESS;
}

CDMi_RESULT CreateMediaEngineSession(
    const IMediaKeySession *f_piMediaKeySession,
    const uint8_t *f_pbCertChain,
    uint32_t f_cbCertChain,
    uint8_t **f_ppbEncryptedSessionKey,
    uint32_t *f_pcbEncryptedSessionKey,
    IMediaEngineSession **f_ppiMediaEngineSession) {
  CMediaEngineSession *poMediaEngineSession = NULL;

  poMediaEngineSession = new CMediaEngineSession();

  // Pass the clear AES session key to MediaEngineSession for sample protection.
  poMediaEngineSession->Init(f_piMediaKeySession, 0, 0);

  *f_ppiMediaEngineSession =
      static_cast<IMediaEngineSession *>(poMediaEngineSession);

  return CDMi_SUCCESS;
}

CDMi_RESULT DestroyMediaEngineSession(
    IMediaEngineSession *f_piMediaEngineSession) {}

}  // namespace CDMi
