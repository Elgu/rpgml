
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

LDFLAGS=

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
	g++ -o $@ $(LDFLAGS) $(OBJECTS)

.parser_files: rpgml.yy
	bison -v rpgml.yy
	touch $@

$(PARSER_FILES): .parser_files

dep: $(SOURCE)
	g++ -M -MP -MF $@ $(CXXFLAGS) $(SOURCE)

clean:
	rm -f $(PARSER_FILES)
	rm -f .parser_files
	rm -f dep
	rm -f *.o
	rm -f rpgml

.PHONY: check

check: rpgml.output
	~/bin/gramdiag rpgml.output

-include dep

