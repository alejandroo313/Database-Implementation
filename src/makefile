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

bestfit:
	@echo Running library database with best_fit strategy
	@./library best_fit test

worstfit:
	@echo Running library database with worst_fit strategy
	@./library worst_fit test

firstfit:
	@echo Running library database with first_fit strategy
	@./library first_fit test

clean:
	rm -f *.o core $(EXE)

delete:
	@echo Deleting all database files
	rm *.db *.ind *.lst