CXX=g++
CCFLAGS= -g -std=c++11 -Wall -Werror
LDLIBS= -lm

all: proj1

# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)
%.o : %.cpp
	$(CXX) -c $(CCFLAGS) $<

# lets-talk.o: lets-talk.c list.c
# 	g++ -g -c lets-talk.c

# eventlist.o: eventlist.cpp
# 	g++ -g -c eventlist.cpp

# eventnode.o: eventnode.cpp
# 	g++ -g -c eventnode.cpp

# patientnode.o: patientnode.cpp
# 	g++ -g -c patientnode.cpp

# queue.o: queue.cpp
# 	g++ -g -c queue.cpp

# room.o: room.cpp
# 	g++ -g -c room.cpp

proj1: main.o eventlist.o eventnode.o patientnode.o queue.o room.o
	$(CXX) -o proj1 main.o eventlist.o eventnode.o patientnode.o queue.o room.o $(CCFLAGS) $(LDLIBS)

clean:
	rm -f core *.o proj1