#include <stdio.h>
#include <stdlib.h>

typedef struct _Indexbook Indexbook;
typedef struct _Array Array;

int add(FILE *db, FILE *ind, char *arguments, Array *a);
void printInd(Array *a);
void Exit(FILE *db, FILE *ind, char *database, char *index, Array *a);
void Saveinfile(FILE *ind, Array *a);

void initArray(Array *a, size_t initialSize);
void insertArray(Array *a, Indexbook element);
void freeArray(Array *a);