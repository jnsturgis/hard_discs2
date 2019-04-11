CC = g++
CFLAGS = -Wall
LIB_FLAGS = 
EXEC_NAME = config2eps
SRC = $(wildcard *.cpp ../Classes/*.cpp)
OBJ = $(SRC:.cpp=.o) 

all : $(EXEC_NAME)

config2eps : $(OBJ)
	$(CC) -o $@ $^ -lboost_program_options

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(EXEC_NAME) $(OBJ)

