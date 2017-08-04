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

// For the support of portable data types such as uint8_t.
#include <stdint.h>

namespace CDMi
{

// The status code returned by CDMi APIs.
typedef int32_t CDMi_RESULT;

#define CDMi_SUCCESS            ((CDMi_RESULT)0)
#define CDMi_S_FALSE            ((CDMi_RESULT)1)
#define CDMi_E_OUT_OF_MEMORY    ((CDMi_RESULT)0x80000002)
#define CDMi_E_FAIL             ((CDMi_RESULT)0x80004005)
#define CDMi_E_INVALID_ARG      ((CDMi_RESULT)0x80070057)

// More CDMi status codes can be defined. In general
// CDMi status codes should use the same PK error codes.

#define CDMi_FAILED(Status)     ((CDMi_RESULT)(Status)<0)
#define CDMi_SUCCEEDED(Status)  ((CDMi_RESULT)(Status) >= 0)

// IMediaKeySessionCallback defines the callback interface to receive
// events originated from MediaKeySession.
class IMediaKeySessionCallback
{
public:
    virtual ~IMediaKeySessionCallback(void) {}

    // Event fired when a key message is successfully created.
    virtual void OnKeyMessage(
        const uint8_t *f_pbKeyMessage, //__in_bcount(f_cbKeyMessage) 
        uint32_t f_cbKeyMessage, //__in 
        char *f_pszUrl) = 0; //__in_z_opt 

    // Event fired when MediaKeySession has found a usable key.
    virtual  void OnKeyReady(void) = 0;

    // Event fired when MediaKeySession encounters an error.
    virtual void OnKeyError(
        CDMi_RESULT f_error) = 0; //__in 
};

// IMediaKeySession defines the MediaKeySession interface.
class IMediaKeySession
{
public:
    virtual ~IMediaKeySession(void) {}

    // Kicks off the process of acquiring a key. A MediaKeySession callback is supplied
    // to receive notifications during the process.
    virtual void Run(
        const IMediaKeySessionCallback *f_piMediaKeySessionCallback) = 0; //__in 

    // Process a key message response.
    virtual void Update(
        const uint8_t *f_pbKeyMessageResponse, //__in_bcount(f_cbKeyMessageResponse) 
        uint32_t f_cbKeyMessageResponse) = 0; //__in 

    // Explicitly release all resources associated with the MediaKeySession.
    virtual void Close(void) = 0;

    // Return the session ID of the MediaKeySession. The returned pointer
    // is valid as long as the associated MediaKeySession still exists.
    virtual const wchar_t *GetSessionId(void) const = 0;

    // Return the key system of the MediaKeySession.
    virtual const wchar_t *GetKeySystem(void) const = 0;
};

// IMediaKeys defines the MediaKeys interface.
class IMediaKeys
{
public:
    virtual ~IMediaKeys(void) {}

    // Check whether the MediaKey supports a specific mime type (optional)
    // and a key system.
    virtual bool IsTypeSupported(
        const wchar_t *f_pwszMimeType, //__in_z_opt 
        const wchar_t *f_pwszKeySystem) const = 0; //__in_z 

    // Create a MediaKeySession using the supplied init data and CDM data.
    virtual CDMi_RESULT CreateMediaKeySession(
        const char *f_pwszMimeType, //__in_z_opt const wchar_t
        const uint8_t *f_pbInitData, //__in_bcount_opt(cbInitData) 
        uint32_t f_cbInitData, //__in 
        const uint8_t *f_pbCDMData, //__in_bcount_opt(f_cbCDMData) 
        uint32_t f_cbCDMData, //__in 
        IMediaKeySession **f_ppiMediaKeySession) = 0; //__deref_out 

    // Destroy a MediaKeySession instance.
    virtual CDMi_RESULT DestroyMediaKeySession(
        IMediaKeySession *f_piMediaKeySession) = 0; //__in 
};

// Global factory method that creates a MediaKeys instance.
CDMi_RESULT CreateMediaKeys(
    IMediaKeys **f_ppiMediaKeys); //__deref_out 

// Global method that destroys a MediaKeys instance.
CDMi_RESULT DestroyMediaKeys(
    IMediaKeys *f_piMediaKeys); //__in 

// IMediaEngineSession represents a secure channel between media engine and CDMi
// for the purpose of sample decryption.
//
// Note: This interface below is at the prototype stage and it will undergo further design 
// review to be finalized.
class IMediaEngineSession
{
public:
    // Decrypt a buffer in-place and internally immediately re-encrypt it using a 
    // session key. In other words the returned buffer can only be encrypted by
    // a party that knows the session key.
    virtual CDMi_RESULT Decrypt(
        const uint8_t *f_pbIV, //__in_bcount(f_cbIV) 
        uint32_t f_cbIV, //__in 
        uint8_t *f_pbData, //__inout_bcount(f_cbData) 
        uint32_t f_cbData, //__in 
        const uint32_t *f_pdwSubSampleMapping, // __in_ecount_opt(f_cdwSubSampleMapping)
        uint32_t f_cdwSubSampleMapping) = 0; //__in 
};

// Global factory method that creates a MediaEngineSession instance.
// pbCert/cbCert is a sample protection certificate chain from a trusted authority.
// After the certificate chain is validated a random session key is generated and then
// encrypted using the public key extracted from the leaf certificate of the sample
// protection certificate chain before sending back to the caller. The session key is
// later used by media engine to decrypt samples that are re-encrypted by the call of
// IMediaEngineSession::decrypt.
//
// Note: This interface below is at the prototype stage and it will undergo further design 
// review to be finalized.
CDMi_RESULT CreateMediaEngineSession(
    const IMediaKeySession *f_piMediaKeySession, //__in 
    const uint8_t *f_pbCertChain, //__in_bcount(f_cbCertChain)
    uint32_t f_cbCertChain, //__in 
    uint8_t **f_ppbEncryptedSessionKey, //__deref_out_bcount(*f_pcbSessionKey) 
    uint32_t *f_pcbEncryptedSessionKey, //__out 
    IMediaEngineSession **f_ppiMediaEngineSession); //__deref_out 

// Global method that destroys a MediaEngineSession instance.
CDMi_RESULT DestroyMediaEngineSession(
    IMediaEngineSession *f_piMediaEngineSession); //__in 

} // namespace CDMi
