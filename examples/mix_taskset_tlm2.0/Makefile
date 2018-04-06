
include ../../Makefile.macros


DEFINES	= -DNDEBUG -DSC_INCLUDE_DYNAMIC_PROCESSES
#DEFINES	= -DSC_INCLUDE_DYNAMIC_PROCESSES
CFLAGS = -g -O2 -Wall $(DEFINES) -I. -I$(HCSIM)/include -I$(SYSTEMC_DIR)/include -I$(TLM_INC)
LFLAGS = -L$(SYSTEMC_DIR)/lib-linux64 -lsystemc -L$(HCSIM)/lib -lhcsim \
	 -Wl,-rpath,$(SYSTEMC_DIR)/lib-linux64 

all: main

test: main
	-$(TIME) ./main

main: main.o HInterruptGenerator_TLM.o 
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

main.o:  
	$(CC) $(CFLAGS) -c main.cpp

HInterruptGenerator_TLM.o: HInterruptGenerator_TLM.cpp 
	$(CC) $(CFLAGS) -c $<

clean:
	-$(RM) *.o main *~ dump*
