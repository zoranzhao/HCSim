
include ../../Makefile.macros


HCSIM = ../../../HCSim
DEFINES	= -DNDEBUG -DSC_INCLUDE_DYNAMIC_PROCESSES

CFLAGS = -g -O2  $(DEFINES) -I. -I$(HCSIM)/include -I$(SYSTEMC_DIR)/include -I$(TLM_INC)
LFLAGS = -L$(SYSTEMC_DIR)/lib-linux64  -L$(HCSIM)/lib -lhcsim -lsystemc\
	 -Wl,-rpath,$(SYSTEMC_DIR)/lib-linux64 -Wl,-rpath,$(HCSIM)/lib 


all: main

test: main
	-$(TIME) ./main

main: main.o  
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

main.o:  
	$(CC) $(CFLAGS) -c main.cpp

clean:
	-$(RM) *.o main *~ dump*
