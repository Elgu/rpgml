
include ../Makefile.defines
include ../Makefile.rules

.PHONY: all clean undepend

#%.source: %.executable
#	@echo Getting SOURCE from $<
#	@grep SOURCE $< | sed "s/^.*SOURCE.*=//" > $@
#
#%.objects: %.source
#	@echo $(addsuffix .o, $(basename $(shell cat $<) ) ) > $@
#
#%.prequisites: %.objects
#
#%: %.prequisites $(shell cat $*.objects)
#	g++ -o $@ $(shell cat $*.objects) $(LDFLAGS) $(shell cat $(addsuffix .LDFLAGS, $(shell cat $*.objects) ) )


all:\
 	rpgml\
	prettyprinter\
	performance_Array\

rpgml_SOURCE=\
	main.cpp\

rpgml_OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(rpgml_SOURCE)))
rpgml_LDFLAGS_files=$(addsuffix .LDFLAGS, $(rpgml_OBJECTS) )

rpgml: $(rpgml_OBJECTS) $(rpgml_LDFLAGS_files)
	g++ -o $@ $(rpgml_OBJECTS) $(LDFLAGS) -L. -lRPGML $(shell cat $(rpgml_LDFLAGS_files))

prettyprinter_SOURCE=\
	prettyprinter.cpp\

prettyprinter_OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(prettyprinter_SOURCE)))
prettyprinter_LDFLAGS_files=$(addsuffix .LDFLAGS, $(prettyprinter_OBJECTS) )

prettyprinter: $(prettyprinter_OBJECTS) $(prettyprinter_LDFLAGS_files)
	g++ -o $@ $(prettyprinter_OBJECTS) $(LDFLAGS) -L. -lRPGML $(shell cat $(prettyprinter_LDFLAGS_files))

performance_Array_SOURCE=\
	performance_Array.cpp\

performance_Array_OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(performance_Array_SOURCE)))
performance_Array_LDFLAGS_files=$(addsuffix .LDFLAGS, $(performance_Array_OBJECTS) )

performance_Array: $(performance_Array_OBJECTS) $(performance_Array_LDFLAGS_files)
	g++ -o $@ $(performance_Array_OBJECTS) $(LDFLAGS) -L. -lRPGML $(shell cat $(performance_Array_LDFLAGS_files))

SOURCE=\
	$(rpgml_SOURCE)\
	$(prettyprinter_SOURCE)\
	$(performance_Array_SOURCE)\

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
	rm -f prettyprinter $(prettyprinter_OBJECTS)

include ../Makefile.stest

include .depend

