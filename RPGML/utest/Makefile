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
	String.o\
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
	Exception.o\
	Mutex.o\
	Semaphore.o\
	JobQueue.o\
	Graph.o\
	rpgml.tab.o\
	utest_Exception.o\
	utest_Type.o\
	utest_String.o\
	utest_Array.o\
	utest_GarbageCollector.o\
	utest_Thread.o\
	utest_JobQueue.o\
	utest_Node.o\
	utest_Graph.o\

%.o: %.cpp .%.dep
	g++ -c -o $@ $(CFLAGS) $<

%.o: %.cc .%.dep
	g++ -c -o $@ $(CFLAGS) $<

.%.dep: %.cpp Makefile
	@echo Generating dependency for $<
	@g++ -M -MP -MF $@ $(CFLAGS) $<

.%.dep: %.cc Makefile
	@echo Generating dependency for $<
	@g++ -M -MP -MF $@ $(CFLAGS) $<

all: utest_main

test: utest_main
	./utest_main

clean:
	rm -f utest_main $(OBJECTS)

utest_main: $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(LDFLAGS)

DEP_FILES=$(foreach S, $(basename $(OBJECTS) ), .$(S).dep)

.depend: $(DEP_FILES)
	@cat $(DEP_FILES) > .depend

include .depend

