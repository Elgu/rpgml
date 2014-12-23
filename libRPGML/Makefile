include ../Makefile.defines
include ../Makefile.rules

.PHONY: all clean check undepend
.SUFFIXES: .cpp .o .dep
vpath %.cpp RPGML
vpath %.cc RPGML

all: libRPGML.so

libRPGML_SOURCE=\
	rpgml.tab.cc\
	Scanner.cpp\
	GarbageCollector.cpp\
	Value.cpp\
	String.cpp\
	Frame.cpp\
	Parser.cpp\
	AST_PrettyPrinter.cpp\
	Function.cpp\
	Scope.cpp\
	Context.cpp\
	InterpretingASTVisitor.cpp\
	Sequence.cpp\
	InterpretingFunction.cpp\
	Node.cpp\
	SharedObject.cpp\
	Thread.cpp\
	Exception.cpp\
	Semaphore.cpp\
	Mutex.cpp\
	JobQueue.cpp\
	Graph.cpp\
	ThreadPool.cpp\

libRPGML_OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(libRPGML_SOURCE)))

libRPGML.so: $(libRPGML_OBJECTS)
	g++ -shared -o $@  $(libRPGML_OBJECTS) $(LDFLAGS) -ldl

SOURCE=\
	$(libRPGML_SOURCE)\

PARSER_FILES=\
	RPGML/location.hh\
	RPGML/rpgml.output\
 	RPGML/position.hh\
 	RPGML/rpgml.output\
 	RPGML/rpgml.tab.cc\
 	RPGML/rpgml.tab.hh\
 	RPGML/stack.hh\

.parser_files: RPGML/rpgml.yy
	cd RPGML && bison -v rpgml.yy
	touch $@

$(PARSER_FILES): .parser_files

DEP_FILES=$(foreach s, $(SOURCE), .$(s).dep)

test:
	@echo DEP_FILES=$(DEP_FILES)

.depend: $(DEP_FILES)
	@cat $(DEP_FILES) > .depend

undepend:
	rm -f $(DEP_FILES) .depend

clean:
	$(MAKE) -C utest clean
	rm -f $(DEP_FILES)
	rm -f $(PARSER_FILES)
	rm -f .parser_files
	rm -f .depend
	rm -f *.o
	rm -f rpgml $(rpgml_OBJECTS)
	rm -f libRPGML.so $(libRPGML_OBJECTS)
	rm -f $(foreach so, $(SOs), RPGML_$(so).o libRPGML_$(so).so )

check: rpgml.output
	~/bin/gramdiag rpgml.output

include .depend

