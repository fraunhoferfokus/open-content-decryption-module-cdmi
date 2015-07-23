// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Copyright 2015 Linaro Ltd
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

#ifndef OPENCDM_CDMI_STUB_JSON_WEB_KEY_H
#define OPENCDM_CDMI_STUB_JSON_WEB_KEY_H
#include  "keypairs.h"

namespace media{

const char kKeysTag[] = "keys";
const char kKeyTypeTag[] = "kty";
const char kKeyTypeOct[] = "oct";  // Octet sequence.
const char kAlgTag[] = "alg";
const char kAlgA128KW[] = "A128KW";  // AES key wrap using a 128-bit key.
const char kKeyTag[] = "k";
const char kKeyIdTag[] = "kid";
const char kKeyIdsTag[] = "kids";
const char kBase64Padding = '=';
const char kTypeTag[] = "type";
const char kTemporarySession[] = "temporary";
const char kPersistentLicenseSession[] = "persistent-license";
const char kPersistentReleaseMessageSession[] = "persistent-release-message";

// Based on the corresponding chromium source

// Extracts the JSON Web Keys from a JSON Web Key Set. If |input| looks like
// a valid JWK Set, then true is returned and |keys| and |session_type| are
// updated to contain the values found. Otherwise return false.
bool ExtractKeysFromJWKSet(const std::string& jwk_set,
                                        KeyIdAndKeyPairs* keys,
                                        int session_type);


}//namespace media
#endif
