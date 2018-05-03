CC = g++
CFLAGS = -Wall
LIB_FLAGS = 
EXEC_NAME = makeconfig
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o) ../NVT/config.o ../NVT/o_list.o ../NVT/topology.o ../NVT/force_field.o ../NVT/atom.o ../NVT/object.o

all : $(EXEC_NAME)

makeconfig : $(OBJ)
	$(CC) -o $@ $^ -lboost_program_options

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm $(EXEC_NAME) $(OBJ)
