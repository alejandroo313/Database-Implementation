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

        a = (Array*)malloc(sizeof(Array));
        if(!a){
            fclose(db);
            free(database);
            free(index);
            fclose(ind);
            return 0;
        }
        initArray(a, 2);

        index = (char*)malloc(strlen(argv[2])+strlen(".ind")+1*sizeof(char));
        if(!index){
            free(database);
            return 0;
        }

        strcpy(index, argv[2]);
        strcat(index, ".ind");
        
        if((ind = fopen(index, "r"))!= NULL){
            Loadfromfile(ind, a);
        }

        ind = fopen(index, "wb");
        if(!ind){
            fclose(db);
            free(database);
            free(index);
            return 0;
        }

        database = (char*)malloc(strlen(argv[2])+strlen(".db")+1*sizeof(char));
        if(!database){
            return 0;
        }

        strcpy(database, argv[2]);
        strcat(database, ".db");

        /*db = fopen(database, "ab");
        if(!db){
            free(database);
            free(index);
            return 0;
        }*/

        printf("Type command and argument/s.\n");
        printf("exit\n");
        while (fgets(input, sizeof(input), stdin)) {
            
            token = strtok(input, " ");
            key = token;
            token = strtok(NULL, "");

            if(strcmp("add", key) == 0){
                if(db == NULL){
                    db = fopen(database, "ab");
                    if(!db){
                        free(database);
                        free(index);
                        return 0;
                    }
                }
                ret = add(db, ind, token, a);

                if(ret == -1){
                    printf("Error en el guardado de los datos\n");
                    Exit(db, ind, database, index, a);
                    return 1;
                }
                printf("Record with BookID=%d has been added to the database\n", ret);
                printf("exit\n");
            }else if(strcmp("find", key)== 0){
                ret = find(database, a, atoi(token), 0, a->used);
                if(ret == -1){
                    printf("Record with bookId=%s does not exist\n", token);
                    Exit(db, ind, database, index, a);
                    return 1;
                }
            }else if(strcmp("del", key)== 0){
                printf("del\n");
            }else if(strcmp("exit\n", key)== 0){
                Exit(db, ind, database, index, a);
                break;
            }else if(strcmp("printRec\n", key)== 0){
                PrintRec(database, a);
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
    long unsigned int i;
    
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

    for(i = 0; i<ISBN_LEN+1; i++){
        book->isbn[i] = '\0';
    }

    /*sacamos el isbn de arguments y lo copiamos a la estructura*/
    token = strtok(NULL, "|");
    if(!token){
        free(book->isbn);
        free(book);
        return -1;
    }
    strncpy(book->isbn, token, ISBN_LEN);


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

    length = strlen(token);
    book->titulo = (char*)malloc(length+2*sizeof(char));
    if(!book->titulo){
        free(book->isbn);
        free(book);
        return -1;
    }

    for(i = 0; i<length+2; i++){
        book->titulo[i] = '\0';
    }

    strncpy(book->titulo, token, length);
    strcat(book->titulo, "|");

    

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
    
    length = strlen(token);
    book->editorial = (char*)malloc(length+1*sizeof(char));
    if(!book->editorial){
        free(book->isbn);
        free(book->titulo);
        free(book);
        return -1;
    }

    for(i = 0; i<length+1; i++){
        book->editorial[i] = '\0';
    }

    strncpy(book->editorial, token, length);

    /*obtenemos el tamaño total del registro*/
    book->tamanio = ISBN_LEN + strlen(book->titulo) + strlen(book->editorial) + sizeof(book->id);

    /*printf("id: %d, stlen: %ld\n", book->id, sizeof(book->id));
    printf("isbn: %s, stlen: %ld\n", book->isbn, strlen(book->isbn));
    printf("titulo: %s, stlen: %ld\n", book->titulo, strlen(book->titulo));
    printf("editorial: %s, stlen: %ld\n", book->editorial, strlen(book->editorial));
    printf("tamanio: %ld, stlen: %ld\n", book->tamanio, sizeof(book->tamanio));*/
    /*PROBLEMA AL PASAR STRINGS*/    
    /*copiamos toda la informacion en el fichero binario*/
    ret = fwrite(&book->tamanio, sizeof(size_t), 1, db);
    ret = fwrite(&book->id, sizeof(int), 1, db);
    ret = fwrite(book->isbn, ISBN_LEN, 1, db);
    ret = fwrite(book->titulo, strlen(book->titulo), 1, db);
    ret = fwrite(book->editorial, length, 1, db);

    /*generar indexbook*/
    /*ibook = (Indexbook*)malloc(sizeof(Indexbook));
    if(!ibook) return -1;*/

    ibook.key = book->id;

    fseek(db, 0L, SEEK_END);
    
    ibook.offset = ftell(db)-(book->tamanio+8);
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

int find(char *database, Array *a, int key, int ip, int iu){
    int m;
    long unsigned int i;
    Book *b = NULL;
    size_t ret, length;
    FILE *db = NULL;
    char buffer[256] = {'\0'}, *token = NULL;

    if(!database || !a || key<0 || ip>iu) return -1;

    db = fopen(database, "r");
    if(!db) return -1;

    b = (Book*)malloc(sizeof(Book));
    if(!b){
        fclose(db);
        return -1;
    }

    while(ip<=iu){
        m = (ip+iu)/2;
        if(a->array[m].key == key){
            break;
        }else if(key < a->array[m].key){
            iu = m-1;
        }else{
            ip = m+1;
        }
    }

    if(a->array[m].key != key){
        fclose(db);
        free(b);
        return -1;
    }
    
    fseek(db, a->array[m].offset+sizeof(size_t), SEEK_SET);
    
    b->id = 0;
    ret = fread(&b->id, sizeof(int), 1, db);

    b->isbn = (char*)malloc(ISBN_LEN+1*sizeof(char));

    for(i = 0; i<ISBN_LEN+1; i++){
        b->isbn[i] = '\0';
    }
    ret = fread(b->isbn, ISBN_LEN, 1, db);

    ret = fread(buffer, a->array[m].size-sizeof(int)-ISBN_LEN, 1, db);

    if(ret != 1){
        fclose(db);
        free(b->isbn);
        free(b);
        return -1;
    }
    
    token = strtok(buffer, "|");
    length = strlen(token);

    b->titulo = (char*)malloc(length+1*sizeof(char));
    if(!b->titulo){
        fclose(db);
        free(b->isbn);
        free(b);
        return -1;
    }

    for(i = 0; i<length+1; i++){
        b->titulo[i] = '\0';
    }

    strncpy(b->titulo, token, length);

    token = strtok(NULL, "\0");
    length = strlen(token);

    b->editorial = (char*)malloc(length+1*sizeof(char));
    if(!b->editorial){
        fclose(db);
        free(b->isbn);
        free(b->titulo);
        free(b);
        return -1;
    }

    for(i = 0; i<length+1; i++){
        b->editorial[i] = '\0';
    }

    strncpy(b->editorial, token, length);
    printf("%d|%s|%s|%s\n", b->id, b->isbn, b->titulo, b->editorial);

    free(b->isbn);
    free(b->titulo);
    free(b->editorial);
    free(b);
    fclose(db);

    return 1;
}

void Exit(FILE *db, FILE *ind, char *database, char *index, Array *a){
    Saveinfile(ind, a);
    
    if(a->array != NULL){
        free(a->array);
        free(a);
    }
    
    if(db != NULL){
        fclose(db);
    }
    
    if(ind != NULL){
        fclose(ind);
    }
    
    if(database != NULL){
        free(database);
    }

    if(index != NULL){
        free(index);
    }
    
}

void PrintRec(char *database, Array *a){
    Book *b = NULL;
    FILE *db = NULL;
    size_t length;
    long unsigned int i, j;
    char buffer[256] = {'\0'};

    if(!database || !a) return;

    db = fopen(database, "r");
    if(!db) return;

    b = (Book*)malloc(sizeof(Book));
    if(!b){
        fclose(db);
        return;
    } 

    b->isbn = (char*)malloc(ISBN_LEN+1*sizeof(char));
    if(!b->isbn){
        fclose(db);
        free(b);
        return;
    }

    for(i = 0; i<ISBN_LEN+1; i++){
        b->isbn[i] = '\0';
    }

    for(i=0; i<a->used; i++){
        fseek(db, a->array[i].offset+sizeof(size_t), SEEK_SET);

        fread(&b->id, sizeof(int), 1, db);
        fread(b->isbn, ISBN_LEN, 1, db);
        fread(buffer, a->array[i].size-sizeof(int)-ISBN_LEN, 1, db);

        printf("%d|%s|%s\n", b->id, b->isbn, buffer);

        length = strlen(buffer);
        for(j=0; j<length+1; j++){
            buffer[j] = '\0';
        }
        
    }
    
    free(b->isbn);
    free(b);
    fclose(db);
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

void Loadfromfile(FILE *ind, Array *a){
    Indexbook index;
    size_t ret = 1;

    while(TRUE){
        ret = fread(&index.key, sizeof(int), 1, ind);
        ret = fread(&index.offset, sizeof(long), 1, ind);
        ret = fread(&index.size, sizeof(size_t), 1, ind);

        if(ret == 0){
            break;
        }

        insertArray(a, index);
    }
    fclose(ind);
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