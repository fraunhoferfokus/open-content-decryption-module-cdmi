#
# Copyright 2015 Linaro Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Library dependencies of CDMI module

CDMI_LFLAGSS:=	-lcdmi \
  -ljsmn\
	-lcrypto \
	-Lcdmi-stub \
	-Lcdmi-stub/jsmn

CDMI_DIR := cdmi-stub

CDMILIB_NAME = cdmi

cdmi:
	$(MAKE) -C $(CDMI_DIR)

clean_cdmi:
	$(MAKE) -C $(CDMI_DIR) clean
