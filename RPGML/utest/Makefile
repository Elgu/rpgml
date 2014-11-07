CFLAGS=\
	`cppunit-config --cflags`\
	-O0 -g3\
	-I..\
#	-DGC_DEBUG\

LDFLAGS=\
	`cppunit-config --libs`\
	-lpthread\

.PHONY: all test
.SUFFIXES: .cpp .o .dep
vpath %.cpp .
vpath %.cpp ..
vpath %.cc ..

OBJECTS=\
	utest_main.o\
	GarbageCollector.o\
	Value.o\
	Node.o\
	Frame.o\
	Scope.o\
	SharedObject.o\
	Parser.o\
	InterpretingASTVisitor.o\
	Context.o\
	Function.o\
	Sequence.o\
	InterpretingFunction.o\
	Scanner.o\
	Thread.o\
	rpgml.tab.o\
	utest_String.o\
	utest_Array.o\
	utest_GarbageCollector.o\
	utest_Thread.o\

%.o: %.cpp .%.dep
	g++ -c -o $@ $(CFLAGS) $<

%.o: %.cc .%.dep
	g++ -c -o $@ $(CFLAGS) $<

.%.dep: %.cpp Makefile
	@echo Generating dependency for $<
	g++ -M -MP -MF $@ $(CFLAGS) $<

.%.dep: %.cc Makefile
	@echo Generating dependency for $<
	g++ -M -MP -MF $@ $(CFLAGS) $<

all: utest_main

test: utest_main
	./utest_main

utest_main: $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(LDFLAGS)

include $(foreach S, $(basename $(OBJECTS) ), .$(S).dep)

