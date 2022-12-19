CC=gcc
CFLAGS=-Wall
INC=inc


%.o: %.c $(INC)
	$(CC) -c -o $@ $< $(CFLAGS)

main: *.o $(INC)/*.o
	$(CC) -o main main.o $(INC)/*.o
