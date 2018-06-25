EXECUTABLE = main
CFLAGS = -c -Wall -g3
LDFLAGS = -g3
LIBS = -lsfml-graphics -lsfml-system -lsfml-window

all: $(EXECUTABLE)

run: all
	$(EXECUTABLE) 
main: main.o
	g++ -o $(EXECUTABLE) $(LDFLAGS) main.o  $(LIBS)

main.o: main.cpp ChartPrinter.hpp Line.hpp
	g++ -Wall $(CFLAGS) -o main.o main.cpp
	


clean:
	rm *.o $(EXECUTABLE)

