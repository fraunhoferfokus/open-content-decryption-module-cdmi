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
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "json_web_key.h"
#include "jsmn.h"
#include "keypairs.h"
#define MAX_JSON_TOKENS 2048
#define MAX_KEY_SIZE  2048
#define MAX_KEY_ID_SIZE  2048
namespace media{

/* 
   Base64 decoder based on: base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/
static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}


/* Checks equality of two JSON string with a char. Returns 0 if strings
 * equal. */
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

static void fixUpURLSafeBase64(std::string &str)
{
    std::replace( str.begin(), str.end(), '_', '/');
    std::replace( str.begin(), str.end(), '-', '+');
}

static bool convertStringsToKeyPair(KeyIdAndKeyPair* pair, std::string key,
        std::string keyId)
{
  size_t padding;
  std::string decoded_key, decoded_key_id;
  /* Chromium removes the padding strings from the B64 strings. We need
   * to append them for compatibility with the B64 parsers */
  padding = keyId.length()%4;

  if(padding > 0)
    keyId.append(padding, kBase64Padding);

  padding = key.length()%4;
  if(padding > 0)
    key.append(padding, kBase64Padding);

  fixUpURLSafeBase64(key);
  fixUpURLSafeBase64(keyId);

  decoded_key = base64_decode(key);
  decoded_key_id = base64_decode(keyId);
  *pair = std::make_pair(decoded_key_id, decoded_key);
  return true;
}

bool ExtractKeysFromJWKSet(const std::string& jwk_set,
                           KeyIdAndKeyPairs* keys,
                           int session_type) {
  /*We expect max 128 tokens
   * FIXME: We need a different and safe JSON parser.
   */
  jsmntok_t t[MAX_JSON_TOKENS];
  jsmn_parser parser;
  int result;
  const char* jchr  = &(jwk_set.c_str()[0]);

  std::string algorithm;
  std::string key;
  std::string keyId;
  jsmn_init(&parser);
  result = jsmn_parse(&parser, jchr, jwk_set.size(), t, sizeof(t)/sizeof(t[0]));

  if(result<0) {
    std::cout << "Failed to parse JSON" << jwk_set << std::endl;
    return false;
   }

  if(jsoneq(jchr, &t[1], kKeysTag)!=0) {
    std::cout <<  "Unable to parse JSON. Expected kKeyTag : " << kKeysTag << std::endl;
    return false;
  }

  KeyIdAndKeyPairs local_keys;
  /* Ignore the first 2 tokens */
  for(int i = 2; i < result; i++) {
    if(jsoneq(jchr, &t[i], kAlgTag) == 0 && (i+1) < MAX_JSON_TOKENS) {
      algorithm = std::string(jchr + t[i+1].start, t[i+1].end - t[i+1].start);
      continue;
      }

    if(jsoneq(jchr, &t[i], kKeyTag) == 0 && (i+1) < MAX_JSON_TOKENS) {
       if(key.size() != 0) {
           std::cout << "CDMI supports only one key in JSON message. Got multiple keys." << std::endl;
           return false;
       }
       key = std::string(jchr + t[i+1].start, t[i+1].end - t[i+1].start);
       continue;
       }

    if(jsoneq(jchr, &t[i], kKeyIdTag) == 0 && (i+1) < MAX_JSON_TOKENS) {
       if(keyId.size() != 0) {
           std::cout << "CDMI supports only one keyID in JSON message. Got multiple keys." << std::endl;
           return false;
       }
       keyId = std::string(jchr + t[i+1].start, t[i+1].end - t[i+1].start);
       continue;
       }
     }
  KeyIdAndKeyPair keyPair;
  convertStringsToKeyPair(&keyPair, key, keyId);
  local_keys.push_back(keyPair);

  keys->swap(local_keys);
  return true;
}

}
