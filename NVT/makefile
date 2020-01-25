CC = g++
CFLAGS = -Wall
LIB_FLAGS = -L../Libraries/ -lboost_program_options -lgzstream -lz 
EXEC_NAME = NVT
SRC = $(wildcard *.cpp ../Classes/*.cpp)
OBJ = $(SRC:.cpp=.o)

all : $(EXEC_NAME)

NVT : $(OBJ)
	$(CC) -g -o $@ $^ $(LIB_FLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(EXEC_NAME) $(OBJ)

