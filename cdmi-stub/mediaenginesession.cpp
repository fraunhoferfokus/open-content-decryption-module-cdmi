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

#include <iostream>

#include "imp.h"
#include "json_web_key.h"

using namespace std;

namespace CDMi {

CMediaEngineSession::CMediaEngineSession(void) :
    m_pbSessionKey(NULL),
    m_cbSessionKey(0),
    m_pMediaKeySession(NULL) {}

CMediaEngineSession::~CMediaEngineSession(void) {}

CDMi_RESULT CMediaEngineSession::Decrypt(
        uint32_t  f_cdwSubSampleMapping,
        const uint32_t *f_pdwSubSampleMapping,
        uint32_t  f_cbIV,
        const uint8_t  *f_pbIV,
        uint32_t  f_cbData,
        const uint8_t  *f_pbData,
        uint32_t *f_pcbOpaqueClearContent,
        uint8_t **f_ppbOpaqueClearContent) {

  CMediaKeySession *pMediaKeySession =
      static_cast<CMediaKeySession *>(m_pMediaKeySession);

  return pMediaKeySession->Decrypt(m_pbSessionKey,
      m_cbSessionKey,
      f_pdwSubSampleMapping,
      f_cdwSubSampleMapping,
      f_pbIV,
      f_cbIV,
      f_pbData,
      f_cbData,
      f_pcbOpaqueClearContent,
      f_ppbOpaqueClearContent
);

}
CDMi_RESULT CMediaEngineSession::ReleaseClearContent(
    const uint32_t  f_cbClearContentOpaque,
    uint8_t  *f_pbClearContentOpaque ){

  CMediaKeySession *pMediaKeySession =
      static_cast<CMediaKeySession *>(m_pMediaKeySession);

  return pMediaKeySession->ReleaseClearContent(m_pbSessionKey,
      m_cbSessionKey,
      f_cbClearContentOpaque,
      f_pbClearContentOpaque);

}


CDMi_RESULT CMediaEngineSession::Init(
    const IMediaKeySession *f_piMediaKeySession,
    const uint8_t *f_pbSessionKey,
    uint32_t f_cbSessionKey) {
  m_pMediaKeySession = const_cast<IMediaKeySession *>(f_piMediaKeySession);

  return CDMi_SUCCESS;
}

CDMi_RESULT CreateMediaEngineSession(
    IMediaKeySession *f_piMediaKeySession,
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
