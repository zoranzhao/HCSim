
include ../Makefile.macros

OBJECTS = sc_mutex_os.o handshake_ch.o handshake_os_ch.o PRTOS.o  ambaAhb.o GenericIntrController.o  MCProcessor_FM.o adapters_TLM.o simpleBus_TLM.o GenericIntrController_TLM.o

SHARED_LIB = libhcsim.so
STATIC_LIB = libhcsim.a

#DEFINES	= -DNDEBUG -DSC_INCLUDE_DYNAMIC_PROCESSES
DEFINES	= -DSC_INCLUDE_DYNAMIC_PROCESSES

CC = g++ 
CFLAGS	= -std=c++0x -g -O2 -Wall -Wparentheses -fPIC $(DEFINES) -I. -I../include -I$(SYSTEMC_DIR)/include -I$(TLM_INC)

all: $(SHARED_LIB)

$(SHARED_LIB): $(OBJECTS) 
	$(CC) -shared $^ -o $@

#Sabine
sc_mutex_os.o: Channels/sc_mutex_os.cpp   
	$(CC) $(CFLAGS) -c  $<
#Sabine

adapters_TLM.o: Channels/adapters_TLM.cpp 
	$(CC) $(CFLAGS) -c  $<

simpleBus_TLM.o: Channels/simpleBus_TLM.cpp 
	$(CC) $(CFLAGS) -c  $<

GenericIntrController_TLM.o: Processor/GenericIntrController_TLM.cpp 
	$(CC) $(CFLAGS) -c  $<

MCProcessor_FM.o: Processor/MCProcessor_FM.cpp 
	$(CC) $(CFLAGS) -c  $<

GenericIntrController.o: Processor/GenericIntrController.cpp 
	$(CC) $(CFLAGS) -c  $<

PRTOS.o: OS/PRTOS.cpp 
	$(CC) $(CFLAGS) -c  $<

ambaAhb.o: Channels/ambaAhb.cpp 
	$(CC) $(CFLAGS) -c  $<

handshake_os_ch.o: Channels/handshake_os_ch.cpp 
	$(CC) $(CFLAGS) -c  $<

handshake_ch.o: Channels/handshake_ch.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	-$(RM) $(OBJECTS) *~ 

distclean: clean
	-$(RM) $(SHARED_LIB) $(STATIC_LIB)
