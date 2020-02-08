CC = g++
CFLAGS = -Wall
LIB_FLAGS = -L../Libraries/ -lgzstream -lz 
EXEC_NAME = pcf \
            wrap \
            2DOrder \
            map2eps
            
SRC = $(wildcard ../Classes/*.cpp)
ASRC = $(wildcard *.cpp)

OBJ = $(SRC:.cpp=.o)
AOBJ = $(ASRC:.cpp=.o)

all : $(EXEC_NAME)

pcf : pcf.o $(OBJ)
	$(CC) -o $@ $^

wrap : wrap.o $(OBJ)
	$(CC) -o $@ $^

2DOrder : 2DOrder.o $(OBJ)
	$(CC) -o $@ $^  $(LIB_FLAGS) 

map2eps : map2eps.o
	$(CC) -o $@ $^

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(EXEC_NAME) $(OBJ) $(AOBJ)

