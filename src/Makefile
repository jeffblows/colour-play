
TARGET = main

CC      = gcc
CFLAGS  = -g
LFLAGS  = -lcurses

SRC     = main.c process_command_line.c thrd.c pthread.c
OBJ     = main.o process_command_line.o thrd.o pthread.o
HDRS    = main.h process_command_line.h thrd.h pthread.h

%.o:    %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all:    $(TARGET)


main:   $(OBJ) $(SRC) $(HDR)
	$(CC) -o $@ $(OBJ) $(LFLAGS)

clean:
	rm -f $(OBJ) $(TARGET) *~*

