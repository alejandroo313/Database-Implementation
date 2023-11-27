CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi

HEADERS = library.h

EXE = library main
OBJ = $(EXE)

all: $(EXE) 

# compile all files ending in *.c
%.o: %.c $(HEADERS)
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o core $(EXE)

remove:
	rm *.db *ind