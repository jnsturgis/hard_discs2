CC = g++
CFLAGS = -Wall
LIB_FLAGS = 
EXEC_NAME = pcf \
            wrap
SRC = $(wildcard ../Classes/*.cpp)
OBJ = $(SRC:.cpp=.o)

all : $(EXEC_NAME)

pcf : pcf.o $(OBJ)
	$(CC) -o $@ $^ -lboost_program_options

wrap : wrap.o $(OBJ)
	$(CC) -o $@ $^ -lboost_program_options

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(EXEC_NAME) $(OBJ)

