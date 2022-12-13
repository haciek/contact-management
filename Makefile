CC=gcc
CFLAGS=-Wall
LIBS=lib/

%.o: %.c $(LIBS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o $(LIBS)/debug.o
	$(CC) -o main main.o $(LIBS)debug.o
