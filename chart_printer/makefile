EXECUTABLE = main
CFLAGS = -c -Wall -g3
LDFLAGS = -g3
LIBS = -lsfml-graphics -lsfml-system -lsfml-window

all: $(EXECUTABLE)

run: all
	$(EXECUTABLE) 
main: main.o ChartPrinter.o Line.o
	g++ -o $(EXECUTABLE) $(LDFLAGS) main.o ChartPrinter.o Line.o $(LIBS)

main.o: main.cpp ChartPrinter.hpp Line.hpp
	g++ $(CFLAGS) -o main.o main.cpp

ChartPrinter.o: ChartPrinter.cpp
	g++ $(CFLAGS) -o ChartPrinter.o ChartPrinter.cpp

Line.o: Line.cpp
	g++ $(CFLAGS) -o Line.o Line.cpp


	

clean:
	rm *.o $(EXECUTABLE)

