#include <stdio.h>
#include <stdlib.h>

typedef struct _Indexbook Indexbook;
typedef struct _Array Array;
typedef struct _Indexdeletedbook Indexdeletedbook;

typedef enum {
  FALSE = 0, /*!< False value */
  TRUE = 1   /*!< True value  */
} Bool;

#define NO_STRATEGY -1
#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2

#define INDEX 0
#define DELETED 1

int add(FILE *db, FILE *ind, char *arguments, Array *a);
int find(char *db, Array *a, int key, int ip, int iu);
int Del(Array *a_index, Array *a_deleted, int key, int strategy);
void Exit(FILE *db, FILE *ind, FILE *del, char *database, char *index, char *deleted, Array *a_index, Array *a_deleted, int strategy);
void PrintRec(char *database, Array *a);
void printInd(Array *a);
void printLst(Array *a);

void Saveinfile(FILE *pf, Array *a, int file_type, int strategy);
void Loadfromfile(FILE *pf, Array *a, int file_type);

void initArray(Array *a, size_t initialSize);
void insertArray(Array *a, void *element, int array_type, int strategy);
void freeArray(Array *a);