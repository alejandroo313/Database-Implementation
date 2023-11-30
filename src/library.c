#include "library.h"
#include <string.h>
/*switch key--------terminal key*/

#define ISBN_LEN 16

typedef struct _Book{
    size_t tamanio;
    int id;
    char *isbn;
    char *titulo;
    char *editorial;
}Book;

struct _Indexbook{
    int key ; 
    long int offset ; 
    size_t size;
};

struct _Array{
    Indexbook * array;
    size_t used;
    size_t size;
};


int main( int argc, char *argv[]){
    FILE *db = NULL, *ind = NULL;
    Array *a = NULL;
    char input[512];
    char *token = NULL;
    char *key = NULL;
    int ret = 0;
    char *database = NULL;
    char *index = NULL;
    
    if(argc <3){
        printf("Missing argument\n");
        return 0;
    }else{
        if (strcmp(argv[1], "best_fit") != 0 && strcmp(argv[1], "first_fit") != 0  && strcmp(argv[1], "worst_fit") != 0){
            printf("Unknown search strategy unknown_search_strategy\n");
            return 0;
        }

        database = (char*)malloc(strlen(argv[2])+strlen(".db")+1*sizeof(char));
        if(!database){
            return 0;
        }

        index = (char*)malloc(strlen(argv[2])+strlen(".ind")+1*sizeof(char));
        if(!index){
            free(database);
            return 0;
        }

        strcpy(database, argv[2]);
        strcat(database, ".db");

        db = fopen(database, "wb");
        if(!db){
            free(database);
            free(index);
            return 0;
        }

        strcpy(index, argv[2]);
        strcat(index, ".ind");
        ind = fopen(index, "wb");
        if(!ind){
            fclose(db);
            free(database);
            free(index);
            return 0;
        }

        a = (Array*)malloc(sizeof(Array));
        if(!a){
            fclose(db);
            free(database);
            free(index);
            fclose(ind);
            return 0;
        }
        initArray(a, 2);

        printf("Type command and argument/s.\n");
        printf("exit\n");
        while (fgets(input, sizeof(input), stdin)) {
            
            token = strtok(input, " ");
            key = token;
            token = strtok(NULL, "");

            if(strcmp("add", key) == 0){
                ret = add(db, ind, token, a);
                if(ret == -1){
                    printf("Error en el guardado de los datos\n");
                    free(database);
                    free(index);
                    return 1;
                }
                printf("Record with BookID=%d has been added to the database\n", ret);
                printf("exit\n");
            }else if(strcmp("find", key)== 0){
                printf("find\n");
            }else if(strcmp("del", key)== 0){
                printf("del\n");
            }else if(strcmp("exit\n", key)== 0){
                Exit(db, ind, database, index, a);
                break;
            }else if(strcmp("printRec", key)== 0){
                printf("printRec\n");
            }else if(strcmp("printInd\n", key)== 0){
                printInd(a);
                printf("exit\n");
            }else if(strcmp("printLst", key)== 0){
                printf("printLst\n");
            }else{
                printf("Error, that operation doesn't exists\n");
            }
        printf("Type command and argument/s.");
        }
    return 0;
    }
}

/* 
 * Funcion que añade el registro a la base de datos
 * Si no hay fallos en la ejecucion, la funcion devuelve el BookId
 * Si hay algun error en la ejecucion, la funcion devuelve -1
*/
int add(FILE *db, FILE *ind, char *arguments, Array *a){
    /*hacer la particion del contenido de arguments y guardarlo en books propia de la funcion*/
    /*escribir en el fichero pf en binario los datos y hacer el retorno oportuno*/
    char *token = NULL;
    Book *book = NULL;
    Indexbook ibook;
    int ret = 0;
    size_t length = 0;
    
    if(!db || !arguments || !ind) return -1;

    fseek(db, 0L, SEEK_END);

    book = (Book*)malloc(sizeof(Book));
    if(!book) return -1;
    book->tamanio = 0;

    /*sacamos book_id de arguments y lo copiamos a la estructura*/
    token = strtok(arguments, "|");
    if(!token){
        free(book);
        return -1;
    }
    
    book->id = atoi(token);

    book->isbn = (char*)malloc(ISBN_LEN+1*sizeof(char));
    if(!book->isbn){
        free(book);
        return -1;
    }

    /*sacamos el isbn de arguments y lo copiamos a la estructura*/
    token = strtok(NULL, "|");
    if(!token){
        free(book->isbn);
        free(book);
        return -1;
    }
    strncpy(book->isbn, token, ISBN_LEN);

    length = strlen(token);
    book->titulo = (char*)malloc(length+2*sizeof(char));
    if(!book->titulo){
        free(book->isbn);
        free(book);
        return -1;
    }

    /*sacamos el titulo de arguments, reservamos memoria para
     *el titulo y lo copiamos a la estructura
     */
    token = strtok(NULL, "|");
    if(!token){
        free(book->isbn);
        free(book->titulo);
        free(book);
        return -1;
    }

    strncpy(book->titulo, token, length);
    strcat(book->titulo, "|");

    length = strlen(token);
    book->editorial = (char*)malloc(length+1*sizeof(char));
    if(!book->editorial){
        free(book->isbn);
        free(book->titulo);
        free(book);
        return -1;
    }

    /*sacamos la editorial de arguments, reservamos memoria para
     *la editorial y lo copiamos a la estructura
     */
    token = strtok(NULL, "\n");
    if(!token){
        free(book->isbn);
        free(book->titulo);
        free(book->editorial);
        free(book);
        return -1;
    }
    strncpy(book->editorial, token, length);

    length = strlen(book->editorial);
    /*obtenemos el tamaño total del registro*/
    book->tamanio = ISBN_LEN + strlen(book->titulo) + length + sizeof(book->id);

    /*printf("id: %d, stlen: %ld\n", book->id, sizeof(book->id));
    printf("isbn: %s, stlen: %ld\n", book->isbn, strlen(book->isbn));
    printf("titulo: %s, stlen: %ld\n", book->titulo, strlen(book->titulo));
    printf("editorial: %s, stlen: %ld\n", book->editorial, strlen(book->editorial));
    printf("tamanio: %ld, stlen: %ld\n", book->tamanio, sizeof(book->tamanio));*/
    /*PROBLEMA AL PASAR STRINGS*/    
    /*copiamos toda la informacion en el fichero binario*/
    ret = fwrite(&book->tamanio, sizeof(book->tamanio), 1, db);
    ret = fwrite(&book->id, sizeof(book->id), 1, db);
    ret = fwrite(book->isbn, ISBN_LEN, 1, db);
    ret = fwrite(book->titulo, strlen(book->titulo), 1, db);
    ret = fwrite(book->editorial, length, 1, db);

    /*generar indexbook*/
    /*ibook = (Indexbook*)malloc(sizeof(Indexbook));
    if(!ibook) return -1;*/

    ibook.key = book->id;
    ibook.offset = 0;
    if(a->used != 0){
        ibook.offset = ftell(db)-(book->tamanio+8);
    }
    
    ibook.size = book->tamanio;
    /*insertar indexbook a array*/
    insertArray(a, ibook);

    /*hexdump -c archivo.bin*/

    free(book->isbn);
    free(book->titulo);
    free(book->editorial);
    free(book);

    if(ret != 1) return -1;
    
    return ibook.key;
}

void printInd(Array *a){
    unsigned i;

    for(i=0; i<a->used; i++){
        printf("Entry #%d\n", i);
        printf("    key: #%d\n", a->array[i].key);
        printf("    offset: #%ld\n", a->array[i].offset);
        printf("    size: #%ld\n", a->array[i].size);
    }
}

void Exit(FILE *db, FILE *ind, char *database, char *index, Array *a){
    if(!db || !ind || !database || !index || !a) return;

    Saveinfile(ind, a);

    free(a->array);
    free(a);
    fclose(db);
    fclose(ind);
    free(database);
    free(index);
}

void Saveinfile(FILE *ind, Array *a){
    unsigned i;

    for(i=0; i<a->used; i++){
        /*printf("key en la posicion %d: %d\n", i, a->array[i].key);
        printf("offset en la posicion %d: %ld\n", i, a->array[i].offset);
        printf("tam en la posicion %d: %ld\n", i, a->array[i].size);*/

        fwrite(&a->array[i].key, sizeof(int), 1, ind);
        fwrite(&a->array[i].offset, sizeof(long), 1, ind);
        fwrite(&a->array[i].size, sizeof(size_t), 1, ind);
    }
}

void initArray(Array *a, size_t initialSize){

    a->array = (Indexbook*)malloc(initialSize*sizeof(Indexbook));
    if(!a->array) return;

    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, Indexbook element){
    Indexbook elem;
    int j;
    if(a->used == a->size){
        a->size *=2;
        a->array = realloc(a->array, a->size*sizeof(Indexbook));
    }

    a->array[a->used] = element;

    if(a->used > 0){
        elem = a->array[a->used];
        j = a->used-1;

        while (j>= 0 && a->array[j].key > elem.key){
            a->array[j+1] = a->array[j];
            j--;
        }

        a->array[j+1] = elem;
    }

    a->used++;
}

void freeArray(Array *a){
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}