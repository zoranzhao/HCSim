
include Makefile.macros

TARGET	= libhcsim.so

SUBDIRS	= src bin 

EXAMPLES= examples/mix_taskset examples/mix_taskset_tlm2.0

all:
	set -e ; for d in $(SUBDIRS); do	\
	  $(MAKE) -C $$d ;			\
	done
	-$(LN) ../src/$(TARGET) lib/$(TARGET)

test:
	set -e ; for d in $(EXAMPLES); do	\
	  $(MAKE) -C $$d all test;			\
	done

clean:
	set -e ; for d in $(SUBDIRS) $(EXAMPLES); do	\
	  $(MAKE) -C $$d clean ;			\
	done
	-$(RM) *~
	-$(RM) lib/libhcsim.so
	-$(RM) src/libhcsim.so
	-$(RM) bin/setup.sh


