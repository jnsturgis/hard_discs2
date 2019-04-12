CC = g++
CFLAGS = -Wall
LIB_FLAGS = 
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all : $(OBJ)

atom.o : common.h atom.h

config.o : common.h config.h

force_field.o : common.h force_field.h

integrator.o : common.h integrator.h

o_list.o : common.h o_list.h

object.o : common.h object.h

topology.o : common.h topology.h

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(OBJ)

