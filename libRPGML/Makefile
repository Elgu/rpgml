include ../Makefile.defines
include ../Makefile.rules

.PHONY: all clean undepend depend test check
vpath %.cpp RPGML
vpath %.cc RPGML

all: libRPGML.so

libRPGML_SOURCE=\
	rpgml.tab.cc\
	Backtrace.cpp\
	StringUnifier.cpp\
	Location.cpp\
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
	ParseException.cpp\
	WaitLock.cpp\
	Type.cpp\

libRPGML_OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(libRPGML_SOURCE)))

libRPGML.so: $(libRPGML_OBJECTS)
	g++ -shared -o $@  $(libRPGML_OBJECTS) $(LDFLAGS) -ldl

SOURCE=\
	$(libRPGML_SOURCE)\

PARSER_FILES=\
 	RPGML/rpgml.output\
	RPGML/location.hh\
 	RPGML/position.hh\
 	RPGML/rpgml.tab.cc\
 	RPGML/rpgml.tab.hh\
 	RPGML/stack.hh\

$(firstword $(PARSER_FILES)): RPGML/rpgml.yy
	cd RPGML && bison -v rpgml.yy

$(wordlist 2, $(words $(PARSER_FILES)), $(PARSER_FILES)): $(firstword $(PARSER_FILES))
	@true # do nothing

DEP_FILES=$(foreach s, $(SOURCE), .$(s).dep)

utest:
	$(MAKE) RPGML_SRC_ROOT=$(RPGML_SRC_ROOT) -C utest test

.depend: $(DEP_FILES)
	@cat $(DEP_FILES) > .depend

undepend:
	rm -f $(DEP_FILES) .depend

clean:
	$(MAKE) -C utest clean
	rm -f $(DEP_FILES)
	rm -f $(PARSER_FILES)
	rm -f .depend
	rm -f *.o
	rm -f *.o.CXXFLAGS
	rm -f *.o.LDFLAGS
	rm -f rpgml $(rpgml_OBJECTS)
	rm -f libRPGML.so $(libRPGML_OBJECTS)
	rm -f $(foreach so, $(SOs), RPGML_$(so).o libRPGML_$(so).so )

include ../Makefile.stest

include .depend

