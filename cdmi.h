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

// For the support of portable data types such as uint8_t.
#include <stdint.h>

namespace CDMi
{

// EME error code to which CDMi errors are mapped. Please
// refer to the EME spec for details of the errors
// https://dvcs.w3.org/hg/html-media/raw-file/tip/encrypted-media/encrypted-media.html
#define MEDIA_KEYERR_UNKNOWN        1
#define MEDIA_KEYERR_CLIENT         2
#define MEDIA_KEYERR_SERVICE        3
#define MEDIA_KEYERR_OUTPUT         4
#define MEDIA_KEYERR_HARDWARECHANGE 5
#define MEDIA_KEYERR_DOMAIN         6

// The status code returned by CDMi APIs.
typedef int32_t CDMi_RESULT;

#define CDMi_SUCCESS            ((CDMi_RESULT)0)
#define CDMi_S_FALSE            ((CDMi_RESULT)1)
#define CDMi_E_OUT_OF_MEMORY    ((CDMi_RESULT)0x80000002)
#define CDMi_E_FAIL             ((CDMi_RESULT)0x80004005)
#define CDMi_E_INVALID_ARG      ((CDMi_RESULT)0x80070057)

#define CDMi_E_SERVER_INTERNAL_ERROR    ((CDMi_RESULT)0x8004C600)
#define CDMi_E_SERVER_INVALID_MESSAGE   ((CDMi_RESULT)0x8004C601)
#define CDMi_E_SERVER_SERVICE_SPECIFIC  ((CDMi_RESULT)0x8004C604)

// More CDMi status codes can be defined. In general
// CDMi status codes should use the same PK error codes.

#define CDMi_FAILED(Status)     ((CDMi_RESULT)(Status)<0)
#define CDMi_SUCCEEDED(Status)  ((CDMi_RESULT)(Status) >= 0)

#define ChkCDMi(expr) do {                                  \
            cr = (expr);                                    \
            if( CDMi_FAILED( cr ) )                         \
            {                                               \
                goto ErrorExit;                             \
            }                                               \
        } while(FALSE)

/* Media Key status required by EME */
#define  MEDIA_KEY_STATUS_USABLE 0
#define  MEDIA_KEY_STATUS_INTERNAL_ERROR  1
#define  MEDIA_KEY_STATUS_EXPIRED  2
#define  MEDIA_KEY_STATUS_OUTPUT_NOT_ALLOWED  3
#define  MEDIA_KEY_STATUS_OUTPUT_DOWNSCALED  4
#define  MEDIA_KEY_STATUS_KEY_STATUS_PENDING  5
#define  MEDIA_KEY_STATUS_KEY_STATUS_MAX  KEY_STATUS_PENDING

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
        int16_t f_nError,
        CDMi_RESULT f_crSysError) = 0;

    //Event fired on key status update
    virtual void OnKeyStatusUpdate(const char* keyMessage) = 0;
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
    virtual const char *GetSessionId(void) const = 0;

    // Return the key system of the MediaKeySession.
    virtual const char *GetKeySystem(void) const = 0;
};

// IMediaKeys defines the MediaKeys interface.
class IMediaKeys
{
public:
    virtual ~IMediaKeys(void) {}

    // Check whether the MediaKey supports a specific mime type (optional)
    // and a key system.
    virtual bool IsTypeSupported(
        const char *f_pszMimeType,
        const char *f_pszKeySystem) const = 0;

    // Create a MediaKeySession using the supplied init data and CDM data.
    virtual CDMi_RESULT CreateMediaKeySession(
        const char *f_pszMimeType,
        const uint8_t *f_pbInitData,
        uint32_t f_cbInitData,
        const uint8_t *f_pbCDMData,
        uint32_t f_cbCDMData,
        IMediaKeySession **f_ppiMediaKeySession) = 0;

    // Destroy a MediaKeySession instance.
    virtual CDMi_RESULT DestroyMediaKeySession(
        IMediaKeySession *f_piMediaKeySession) = 0;
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
    virtual ~IMediaEngineSession(void) {}

    // Decrypt a content buffer:
    // The output can be a handle in some protected memory space usable by graphics 
    // system, pointer to decrypted content or sample protected (encrypted) content; 
    // in case of decrypted content it is assumed that the memory space being 
    // operated in is secure from external activities and thus conforms 
    // to the requirement of content being protected. Clear content has to be released 
    // through ReleaseClearContent API. It is up to the implementer of the CDM to make 
    // sure it satisfies all compliance and robustness rules.
    virtual CDMi_RESULT Decrypt(
        uint32_t  f_cdwSubSampleMapping,
        const uint32_t *f_pdwSubSampleMapping,
        uint32_t  f_cbIV,
        const uint8_t  *f_pbIV,
        uint32_t  f_cbData,
        const uint8_t  *f_pbData,
        uint32_t *f_pcbOpaqueClearContent,
        uint8_t **f_ppbOpaqueClearContent) = 0;

    virtual CDMi_RESULT ReleaseClearContent(
        const uint32_t  f_cbClearContentOpaque,
        uint8_t  *f_pbClearContentOpaque ) = 0;
};

// Global factory method that creates a MediaEngineSession instance.
// If the returned media engine session interface is not needed then it must be released
// via the call of DestroyMediaEngineSession. 
// if more information is necessary for instantiation of the class, 
// the implementation of this method can be augmented.
CDMi_RESULT CreateMediaEngineSession(
    IMediaKeySession     *f_piMediaKeySession,
    IMediaEngineSession **f_ppiMediaEngineSession);

// Global method that destroys a MediaEngineSession instance.
CDMi_RESULT DestroyMediaEngineSession(
    IMediaEngineSession *f_piMediaEngineSession); //__in 

} // namespace CDMi
