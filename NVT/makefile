CC = g++
CFLAGS = -Wall
LIB_FLAGS = 
EXEC_NAME = NVT
SRC = $(wildcard *.cpp ../Classes/*.cpp)
OBJ = $(SRC:.cpp=.o)

all : $(EXEC_NAME)

NVT : $(OBJ)
	$(CC) -g -o $@ $^ -lboost_program_options

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(EXEC_NAME) $(OBJ)

