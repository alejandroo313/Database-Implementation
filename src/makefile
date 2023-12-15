CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi

LOBJ = utils.o array.o library.o

EXE = library
OBJ = $(EXE)

all: $(EXE) 

$(EXE): $(LOBJ) 
	$(CC) -o library $(LOBJ) 

library.o: library.c array.h utils.h 
	$(CC) $(CFLAGS) -c library.c

utils.o: utils.c utils.h types.h
	$(CC) $(CFLAGS) -c utils.c

array.o: array.c array.h types.h
	$(CC) $(CFLAGS) -c array.c

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

remove:
	@echo Deleting all database files
	rm *.db *.ind *.lst