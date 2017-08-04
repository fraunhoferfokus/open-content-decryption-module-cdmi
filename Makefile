#
# Copyright (c) 2017 Fraunhofer FOKUS
#
# Licensed under the MIT License (the "License");
# you may not use this file except in compliance with the License.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


CXXFLAGS = -g

RPC_DIR = rpc

RPC_SOURCES = \
      $(RPC_DIR)/gen/opencdm_xdr_xdr.c\
      $(RPC_DIR)/gen/opencdm_xdr_svc.c \
      $(RPC_DIR)/gen/opencdm_callback_xdr.c \
      $(RPC_DIR)/gen/opencdm_callback_clnt.c 

RPC_OBJECTS = $(RPC_SOURCES:.c=.o)
RPC_INCLUDES = -I $(RPC_DIR)/gen

SERVICE_SOURCES = service.cpp libs/shmemsem/shmemsem_helper.cpp
SERVICE_INCLUDES =  $(RPC_INCLUDES)
SERVICE_LIBS = -lrt $(JSMN_LIBS)  $

.PHONY: clean cdmiservice

all:  cdmi cdmiservice

.c.o:
	$(CXX) $(CXXFLAGS) $(RPC_INCLUDES) -c $< -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CDMI_INCLUDES) -c $< -o $@


rpc: $(RPC_OBJECTS)

cdmiservice: $(RPC_OBJECTS)
	$(CXX) $(CXXFLAGS) -pthread -std=c++0x \
		$(CDMI_INCLUDES) $(SERVICE_INCLUDES) -L . \
		-o $@ $(SERVICE_SOURCES) $^ $(SERVICE_LIBS) $(CDMI_LFLAGSS)

clean: clean_cdmi
	rm -f $(RPC_OBJECTS)
	rm -f service

ifndef CDMI_MAKE_CONFIG
include  config_clearkey.mk
else
include $(CDMI_MAKE_CONFIG)
endif
