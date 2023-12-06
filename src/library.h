#include <stdio.h>
#include <stdlib.h>

typedef struct _Indexbook Indexbook;
typedef struct _Array Array;

typedef enum {
  FALSE = 0, /*!< False value */
  TRUE = 1   /*!< True value  */
} Bool;

int add(FILE *db, FILE *ind, char *arguments, Array *a);
int find(char *db, Array *a, int key, int ip, int iu);
void Exit(FILE *db, FILE *ind, char *database, char *index, Array *a);
void PrintRec(FILE *db, Array *a);
void printInd(Array *a);

void Saveinfile(FILE *ind, Array *a);
void Loadfromfile(FILE *ind, Array *a);

void initArray(Array *a, size_t initialSize);
void insertArray(Array *a, Indexbook element);
void freeArray(Array *a);