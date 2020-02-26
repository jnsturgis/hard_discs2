CC = g++
CFLAGS = -Wall
LIB_FLAGS = -L../Libraries/ -lboost_system -lboost_program_options -lboost_filesystem -lgzstream -lz 
EXEC_NAME = NVT RX
SRC = $(wildcard ../Classes/*.cpp)
OBJ = $(SRC:.cpp=.o)

all : $(EXEC_NAME)

NVT : $(OBJ) NVT.o
	$(CC) -g -o $@ $^ $(LIB_FLAGS)

RX  : $(OBJ) RX.o
	$(CC) -g -o $@ $^ $(LIB_FLAGS)

%.o: %.cpp command_line.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(EXEC_NAME) RX.o NVT.o

