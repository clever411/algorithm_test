EXECUTABLE = main
CFLAGS = -c -Wall -g3 -DDEBUG -I../
LDFLAGS =
LIBS = -lsfml-graphics -lsfml-system -lsfml-window -lconfig++





# main
all: $(EXECUTABLE)

run: $(EXECUTABLE)
	$(EXECUTABLE) config


rebuild: clean $(EXECUTABLE)

rerun: clean $(EXECUTABLE)
	$(EXECUTABLE) config



$(EXECUTABLE): main.o Line.o ChartPrinter.o
	g++ $(LDFLAGS) -o $(EXECUTABLE) main.o Line.o ChartPrinter.o $(LIBS)

main.o: main.cpp
	g++ $(CFLAGS) -o main.o main.cpp



Line.o: Line.cpp
	g++ $(CFLAGS) -o Line.o Line.cpp

ChartPrinter.o: ChartPrinter.cpp
	g++ $(CFLAGS) -o ChartPrinter.o ChartPrinter.cpp





# clean
clean:
	-rm *.o $(EXECUTABLE)





# end
