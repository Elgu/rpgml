
include Makefile.defines
include Makefile.rules

SUBDIRS=\
	libRPGML\
	rpgml\
	ROOT\

all:
	$(foreach subdir, $(SUBDIRS), $(MAKE) -C $(subdir) all && ) true

clean:
	$(foreach subdir, $(SUBDIRS), $(MAKE) -C $(subdir) clean && ) true
	rm -f $(STEST_SCRIPTS:.rpgml=.output) $(STEST_SCRIPTS:.rpgml=.pretty)
	find . -name ".rerun_stest" -exec rm {} \;
	find . -name "*.o.CXXFLAGS" -exec rm {} \;
	find . -name "*.o.LDFLAGS" -exec rm {} \;
	find . -name ".*.dep" -exec rm {} \;
	find . -name ".depend" -exec rm {} \;

undepend:
	$(foreach subdir, $(SUBDIRS), $(MAKE) -C $(subdir) undepend && ) true

utest:
	$(MAKE) -C libRPGML/utest test

print:
	@echo "RPGML_SRC_ROOT='$(RPGML_SRC_ROOT)'"

include Makefile.stest

