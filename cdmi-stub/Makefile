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

CXXFLAGS = -g

CDMI_SOURCES = mediakeysession.cpp mediakeys.cpp mediaenginesession.cpp 
CDMI_OBJECTS = $(CDMI_SOURCES:.cpp=.o)
CDMI_INCLUDES = -I ../ -I ./jsmn/
CDMILIB_NAME = cdmi
CDMI_LIBS = -ljsmn -L ./jsmn/  -lcrypto

.PHONY: clean all jsmn

all: cdmi

cdmi: jsmn cdmi_lib

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CDMI_INCLUDES) $(CDMI_LIBS) -c $< -o $@

cdmi_lib: $(CDMI_OBJECTS)
	$(AR) rcs lib$(CDMILIB_NAME).a $^

jsmn:
	$(MAKE) -C jsmn/

jsmn_clean:
	$(MAKE) -C jsmn/ clean

clean: jsmn_clean
	rm -f $(CDMI_OBJECTS)
	rm -f lib$(CDMILIB_NAME).a

