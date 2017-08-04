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

#include "cdmi.h"

using namespace std;

namespace CDMi
{

class CMediaKeySession : public IMediaKeySession
{
public:
    CMediaKeySession(const char *sessionId);

    virtual ~CMediaKeySession(void);

    virtual void Run(
        const IMediaKeySessionCallback *f_piMediaKeySessionCallback);

    void* RunThread(int i);

    virtual void Update(
        const uint8_t *f_pbKeyMessageResponse,
        uint32_t f_cbKeyMessageResponse);

    virtual void Close(void);

    virtual const char *GetSessionId(void) const;

    virtual const char *GetKeySystem(void) const;

    CDMi_RESULT Init(
        const uint8_t *f_pbInitData,
        uint32_t f_cbInitData,
        const uint8_t *f_pbCDMData,
        uint32_t f_cbCDMData);

    // Decrypt is not an IMediaKeySession interface method therefore it can only be
    // accessed from code that has internal knowledge of CMediaKeySession.
    CDMi_RESULT Decrypt(
        const uint8_t *f_pbSessionKey,
        uint32_t f_cbSessionKey,
        const uint32_t *f_pdwSubSampleMapping,
        uint32_t f_cdwSubSampleMapping,
        const uint8_t *f_pbIV,
        uint32_t f_cbIV,
        const uint8_t *f_pbData,
        uint32_t f_cbData,
        uint32_t *f_pcbOpaqueClearContent,
        uint8_t **f_ppbOpaqueClearContent);

    virtual CDMi_RESULT ReleaseClearContent(
        const uint8_t *f_pbSessionKey,
        uint32_t f_cbSessionKey,
        const uint32_t  f_cbClearContentOpaque,
        uint8_t  *f_pbClearContentOpaque );

private:
    static void* _CallRunThread(
        void *arg);

    static void* _CallRunThread2(
        void *arg);

private:
    const char *m_sessionId;
    IMediaKeySessionCallback *m_piCallback;
};

class CMediaKeys : public IMediaKeys
{
public:
    CMediaKeys();

    virtual ~CMediaKeys(void);

    virtual bool IsTypeSupported(
        const char *f_pwszMimeType,
        const char *f_pwszKeySystem) const;

    virtual CDMi_RESULT CreateMediaKeySession(
        const char *f_pwszMimeType,
        const uint8_t *f_pbInitData,
        uint32_t f_cbInitData,
        const uint8_t *f_pbCDMData,
        uint32_t f_cbCDMData,
        IMediaKeySession **f_ppiMediaKeySession);

    virtual CDMi_RESULT DestroyMediaKeySession(
        IMediaKeySession *f_piMediaKeySession);

private:
    static const char * CreateSessionId();

private:
    static uint32_t s_sessionCnt;
};

class CMediaEngineSession : public IMediaEngineSession
{
public:
    CMediaEngineSession(void);

    virtual ~CMediaEngineSession(void);

    virtual CDMi_RESULT Decrypt(
        uint32_t  f_cdwSubSampleMapping,
        const uint32_t *f_pdwSubSampleMapping,
        uint32_t  f_cbIV,
        const uint8_t  *f_pbIV,
        uint32_t  f_cbData,
        const uint8_t  *f_pbData,
        uint32_t *f_pcbOpaqueClearContent,
        uint8_t **f_ppbOpaqueClearContent);

    virtual CDMi_RESULT ReleaseClearContent(
        const uint32_t  f_cbClearContentOpaque,
        uint8_t  *f_pbClearContentOpaque );

    CDMi_RESULT Init(
        const IMediaKeySession *f_piMediaKeySession,
        const uint8_t *f_pbSessionKey,
        uint32_t f_cbSessionKey);

private:
    IMediaKeySession *m_pMediaKeySession;

    uint8_t *m_pbSessionKey;
    uint32_t m_cbSessionKey;
};

}  // namespace CDMi
