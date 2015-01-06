
include ../Makefile.defines
include ../Makefile.rules

.PHONY: all clean undepend

all:\
 	rpgml\

rpgml_SOURCE=\
	main.cpp\

rpgml_OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(rpgml_SOURCE)))
rpgml_LDFLAGS_files=$(addsuffix .LDFLAGS, $(rpgml_OBJECTS) )

rpgml: $(rpgml_OBJECTS) $(rpgml_LDFLAGS_files)
	g++ -o $@ $(rpgml_OBJECTS) $(LDFLAGS) -L. -lRPGML $(shell cat $(rpgml_LDFLAGS_files))

SOURCE=\
	$(rpgml_SOURCE)\

DEP_FILES=$(foreach s, $(SOURCE), .$(s).dep)

.depend: $(DEP_FILES)
	@cat $(DEP_FILES) > .depend

undepend:
	rm -f $(DEP_FILES) .depend

clean:
	rm -f $(DEP_FILES)
	rm -f .depend
	rm -f *.o
	rm -f *.o.CXXFLAGS
	rm -f *.o.LDFLAGS
	rm -f rpgml $(rpgml_OBJECTS)

include .depend

print:
	@echo "RPGML_SRC_ROOT='$(RPGML_SRC_ROOT'"

