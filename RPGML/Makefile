
#.SUFFIXES: .o .cpp .cc .hh .yy .dep

CFLAGS=\
	-Wall\
	-Wextra\
	-Wformat\
	-Winit-self\
	-Wuninitialized\
	-Wsuggest-attribute=pure\
	-Wsuggest-attribute=const\
	-Wsuggest-attribute=noreturn\
	-Wshadow\
	-Wwrite-strings\
	-Wconversion\
	-Wlogical-op\
	-Wswitch\
	-I.. -fPIC\
	-g -O0\
	-DDEBUG\

CXXFLAGS=$(CFLAGS)

LDFLAGS=\
	-lpthread\

.%.cpp.dep: %.cpp
	@echo Generating dependencies for $<
	@g++ -M -MM -MG -MF $@ $(CXXFLAGS) $<

.%.cc.dep: %.cc
	@echo Generating dependencies for $<
	@g++ -M -MM -MG -MF $@ $(CXXFLAGS) $<

libRPGML_%.so: RPGML_%.o libRPGML.so
	g++ -shared -o $@ $(LDFLAGS) -L. -lRPGML $<

SOs=\
		binaryOp\
		print\
		unaryOp\

.PHONY: all

all: rpgml $(foreach so, $(SOs), libRPGML_$(so).so )

rpgml_SOURCE=\
	main.cpp\

rpgml_OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(rpgml_SOURCE)))

rpgml: $(rpgml_OBJECTS) libRPGML.so
	g++ -o $@ $(rpgml_OBJECTS) $(LDFLAGS) -L. -lRPGML

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

libRPGML_OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(libRPGML_SOURCE)))

libRPGML.so: $(libRPGML_OBJECTS)
	g++ -shared -o $@  $(libRPGML_OBJECTS) $(LDFLAGS) -ldl

SOURCE=\
	$(rpgml_SOURCE)\
	$(foreach so, $(SOs), RPGML_$(so).cpp )\
	$(libRPGML_SOURCE)\

PARSER_FILES=\
	location.hh\
	rpgml.output\
 	position.hh\
 	rpgml.output\
 	rpgml.tab.cc\
 	rpgml.tab.hh\
 	stack.hh\

.parser_files: rpgml.yy
	bison -v rpgml.yy
	touch $@

$(PARSER_FILES): .parser_files

DEP_FILES=$(foreach s, $(SOURCE), .$(s).dep)

test:
	@echo DEP_FILES=$(DEP_FILES)

.depend: $(DEP_FILES)
	cat $(DEP_FILES) > .depend

clean:
	rm -f $(DEP_FILES)
	rm -f $(PARSER_FILES)
	rm -f .parser_files
	rm -f .depend
	rm -f *.o
	rm -f rpgml $(rpgml_OBJECTS)
	rm -f libRPGML.so $(libRPGML_OBJECTS)
	rm -f $(foreach so, $(SOs), RPGML_$(so).o libRPGML_$(so).so )

.PHONY: check

check: rpgml.output
	~/bin/gramdiag rpgml.output

include .depend

