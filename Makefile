
include Makefile.defines
include Makefile.rules

.SUFFIXES: .all .clean .undepend .depend

SUBDIRS=\
	libRPGML\
	rpgml\
	ROOT\

.%.all: %
	$(MAKE) -C $< all

.%.clean: %
	$(MAKE) -C $< clean

.%.undepend: %
	$(MAKE) -C $< undepend

all: $(foreach subdir, $(SUBDIRS), .$(subdir).all )

.rpgml.all: .libRPGML.all

.ROOT.all: .libRPGML.all

clean: $(foreach subdir, $(SUBDIRS), .$(subdir).clean )
	rm -f $(STEST_SCRIPTS:.rpgml=.output) $(STEST_SCRIPTS:.rpgml=.pretty)
	find . -name ".rerun_stest" -exec rm {} \;
	find . -name "*.o.CXXFLAGS" -exec rm {} \;
	find . -name "*.o.LDFLAGS" -exec rm {} \;
	find . -name ".*.dep" -exec rm {} \;
	find . -name ".depend" -exec rm {} \;

undepend: $(foreach subdir, $(SUBDIRS), .$(subdir).undepend )

utest:
	$(MAKE) -C libRPGML/utest test

print:
	@echo "RPGML_SRC_ROOT='$(RPGML_SRC_ROOT)'"

include Makefile.stest

