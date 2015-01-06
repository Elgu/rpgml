
RPGML_SRC_ROOT=$(shell pwd)

SUBDIRS=\
	libRPGML\
	rpgml\
	ROOT\

all:
	$(foreach subdir, $(SUBDIRS), $(MAKE) RPGML_SRC_ROOT=$(RPGML_SRC_ROOT) -C $(subdir) all && ) true

clean:
	$(foreach subdir, $(SUBDIRS), $(MAKE) RPGML_SRC_ROOT=$(RPGML_SRC_ROOT) -C $(subdir) clean && ) true

undepend:
	$(foreach subdir, $(SUBDIRS), $(MAKE) RPGML_SRC_ROOT=$(RPGML_SRC_ROOT) -C $(subdir) undepend && ) true

test:
	$(MAKE) RPGML_SRC_ROOT=$(RPGML_SRC_ROOT) -C libRPGML/utest test

print:
	@echo "RPGML_SRC_ROOT='$(RPGML_SRC_ROOT)'"
