#
# makefile for compiling and running tests
#

CC = g++
CFLAGS = -Wall
LIB_FLAGS = 
SRC = $(wildcard *_test.cpp)
OBJ = $(SRC:.cpp=.o)
TESTS = polygon_test \
        config_test  \
        topology_test

all : $(OBJ) $(TESTS)

test:
	./test.sh

topology_test.o: ../Classes/topology.h
polygon_test.o: ../Classes/polygon.h
config_test.o: ../Classes/config.h

polygon_test: polygon_test.o ../Classes/polygon.o
	$(CC) -g -o $@ $^

topology_test: topology_test.o ../Classes/topology.o ../Classes/atom.o ../Classes/molecule.o
	$(CC) -g -o $@ $^

config_test: config_test.o ../Classes/config.o ../Classes/polygon.o ../Classes/object.o  ../Classes/o_list.o  ../Classes/atom.o ../Classes/molecule.o ../Classes/force_field.o ../Classes/topology.o
	$(CC) -g -o $@ $^

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(OBJ)

