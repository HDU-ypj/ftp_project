CC = gcc
BIN = project
OBJ = tools.o function.o main.o
FLAG = -Wall_Werror
STD = std=gnu99

all:$(OBJ)
	$(CC) -o $(BIN) $(OBJ) && mv *.o ./bin && ./$(BIN)

main.o: main.c function.h tools.h
	$(CC) -c main.c -o main.o
function.o:function.c function.h tools.h
	$(CC) -c function.c -o function.o
tools.o:tools.c tools.h function.h
	$(CC) -c tools.c -o tools.o

clean:
	rm -rf $(BIN) ./bin/*.o $(OBJ)
	
