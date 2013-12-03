#
# Copyright 2014 Fraunhofer FOKUS
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

CDMI_DIR = cdmi-stub
CDMI_SOURCES = $(CDMI_DIR)/mediakeysession.cpp $(CDMI_DIR)/mediakeys.cpp $(CDMI_DIR)/mediaenginesession.cpp
CDMI_OBJECTS = $(CDMI_SOURCES:.cpp=.o)
CDMI_INCLUDES = -I .
CDMILIB_NAME = cdmi

RPC_DIR = rpc
RPC_SOURCES = $(RPC_DIR)/gen/opencdm_xdr_xdr.c $(RPC_DIR)/gen/opencdm_xdr_svc.c $(RPC_DIR)/gen/opencdm_callback_xdr.c $(RPC_DIR)/gen/opencdm_callback_clnt.c 
RPC_OBJECTS = $(RPC_SOURCES:.c=.o)
RPC_INCLUDES = -I $(RPC_DIR)/gen

SERVICE_SOURCES = service.cpp libs/shmemsem/shmemsem_helper.cpp
SERVICE_INCLUDES = -I $(CDMI_DIR) $(RPC_INCLUDES)
SERVICE_LIBS = -l$(CDMILIB_NAME) -lrt

.PHONY: clean

all: cdmi rpc service

.c.o:
	$(CXX) $(CXXFLAGS) $(RPC_INCLUDES) -c $< -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CDMI_INCLUDES) -c $< -o $@

cdmi: $(CDMI_OBJECTS)
	$(AR) rcs lib$(CDMILIB_NAME).a $^

rpc: $(RPC_OBJECTS)

service: $(RPC_OBJECTS)
	$(CXX) $(CXXFLAGS) -pthread -std=c++0x \
		$(CDMI_INCLUDES) $(SERVICE_INCLUDES) -L . \
		-o $@ $(SERVICE_SOURCES) $^ $(SERVICE_LIBS)

clean:
	rm $(CDMI_OBJECTS)
	rm $(RPC_OBJECTS)
	rm lib$(CDMILIB_NAME).a
	rm service
