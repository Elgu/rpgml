
#.SUFFIXES: .o .cpp .cc .hh .yy .dep

SOURCE=\
	rpgml.tab.cc\
	Scanner.cpp\
	GarbageCollector.cpp\
	Array.cpp\
	Value.cpp\
	Map.cpp\
	Parser.cpp\
	AST_PrettyPrinter.cpp\
	Function.cpp\
	Scope.cpp\
	Context.cpp\
	InterpretingASTVisitor.cpp\
	Sequence.cpp\
	InterpretingFunction.cpp\
	UnaryOp.cpp\
	Print.cpp\
	Node.cpp\
	BinaryOp.cpp\
	SharedObject.cpp\
	main.cpp\

OBJECTS=$(patsubst %.cc,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))

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
	-ldl\

PARSER_FILES=\
	location.hh\
	rpgml.output\
 	position.hh\
 	rpgml.output\
 	rpgml.tab.cc\
 	rpgml.tab.hh\
 	stack.hh\

.PHONY: all

all: rpgml

rpgml: $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(LDFLAGS)

.parser_files: rpgml.yy
	bison -v rpgml.yy
	touch $@

$(PARSER_FILES): .parser_files

.%.cpp.dep: %.cpp
	@echo Generating dependencies for $<
	@g++ -M -MM -MG -MF $@ $(CXXFLAGS) $<

.%.cc.dep: %.cc
	@echo Generating dependencies for $<
	@g++ -M -MM -MG -MF $@ $(CXXFLAGS) $<

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
	rm -f rpgml

.PHONY: check

check: rpgml.output
	~/bin/gramdiag rpgml.output

include .depend

