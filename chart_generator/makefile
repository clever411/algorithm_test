EXECUTABLE = main
CFLAGS = -c -Wall -O5
LDFLAGS = -O5
LIBS = -lconfig++

all: $(EXECUTABLE)

run: all
	$(EXECUTABLE) 

rebuild: clean all

rerun: clean run



main: main.o
	g++ -o $(EXECUTABLE) $(LDFLAGS) main.o $(LIBS)


main.o: main.cpp
	g++ $(CFLAGS) -o main.o main.cpp




clean:
	rm *.o $(EXECUTABLE)

