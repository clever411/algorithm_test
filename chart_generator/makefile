EXECUTABLE = main
CFLAGS = -c -Wall -O5
LDFLAGS = 
LIBS =





# main
all: $(EXECUTABLE)

run: $(EXECUTABLE)
	$(EXECUTABLE) 


rebuild: clean $(EXECUTABLE)

rerun: clean $(EXECUTABLE)
	$(EXECUTABLE)



$(EXECUTABLE): main.o
	g++ $(LDFLAGS) -o $(EXECUTABLE) main.o $(LIBS)

main.o: main.cpp
	g++ $(CFLAGS) -o main.o main.cpp





# clean
clean:
	-rm *.o $(EXECUTABLE)





# end
