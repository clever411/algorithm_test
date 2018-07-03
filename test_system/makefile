EXECUTABLE = algtest
CFLAGS = -c -Wall
LDFLAGS =
LIBS = -lconfig++

all: $(EXECUTABLE)

run: all
	$(EXECUTABLE) 

rebuild: clean all

rerun: clean run



algtest: algtest.o
	g++ -o $(EXECUTABLE) $(LDFLAGS) algtest.o $(LIBS)


algtest.o: algtest.cpp
	g++ $(CFLAGS) -o algtest.o algtest.cpp




clean:
	rm *.o $(EXECUTABLE)

