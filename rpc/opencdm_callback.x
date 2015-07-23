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

/*
 * OpenCDM XDR to be used for RPC communication between DRM and CDM platform counterpart
 * Based on EME methods and naming.
 */

 struct rpc_cb_message {
    char session_id <>;
    string message <>;
    string destination_url <>;
};

struct rpc_cb_key_status_update {
    char session_id <>;
    string message <>;
};


struct rpc_cb_ready {
    char session_id <>;
};


struct rpc_cb_error {
    char session_id <>;
    int error;
    string error_message <>;
};

program OPEN_CDM_CALLBACK {
    version OPEN_CDM_EME_5 {
    void ON_MESSAGE(rpc_cb_message) = 1;
    void ON_READY(rpc_cb_ready) = 2;
    void ON_ERROR(rpc_cb_error) = 3;
    void ON_KEY_STATUS_UPDATE(rpc_cb_key_status_update) = 4;
    } = 1;
} = 0x66666666;
