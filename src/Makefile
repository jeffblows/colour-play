
TARGET = main

CC	= gcc
CFLAGS	= -g

SRC	= main.c process_command_line.c
OBJ	= main.o process_command_line.o
HDRS	= main.h process_command_line.h

%.o:	%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

all:	$(TARGET)


main:	$(OBJ) $(SRC) $(HDR)
	$(CC) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

