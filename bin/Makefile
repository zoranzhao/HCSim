
include ../Makefile.macros

TARGETS	= setup.sh

all: $(TARGETS)

setup.sh : setup.sh.in
	PWD=`pwd` ; BASEDIR=`dirname "$$PWD"` ;		\
	$(SED) -e "s|@HCSIM@|$$BASEDIR|g"			\
	       < $< > $@ ;					\
	$(CHMOD) +x $@

clean:
	-$(RM) *~ 

distclean: clean
	-$(RM) $(TARGETS)
