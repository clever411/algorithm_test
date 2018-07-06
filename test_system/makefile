EXECUTABLE = main
CFLAGS = -c -Wall -O5 -I../lib
LDFLAGS = 
LIBS =
ALGORITHM = -DMERGE_SORT





# algorithm test with writing config file
all: $(EXECUTABLE)

run: all
	$(EXECUTABLE) 

rebuild: clean all

rerun: clean run



$(EXECUTABLE): main.o
	g++ $(LDFLAGS) -o $(EXECUTABLE) main.o $(LIBS)

main.o: main.cpp
	g++ $(CFLAGS) $(ALGORITHM) -o main.o main.cpp





# algorithm test without writing config file
check: clean check.cpp
	g++ -g3 $(ALGORITHM) -o check check.cpp

checkrun: clean check
	check





# clean
clean:
	-rm *.o $(EXECUTABLE) check





# end
