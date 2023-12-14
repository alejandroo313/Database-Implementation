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
    void **array;
    size_t used;
    size_t size;
};

struct _Indexdeletedbook{
    size_t register_size;
    size_t offset;
};

int main( int argc, char *argv[]){
    FILE *db = NULL, *ind = NULL, *del = NULL;
    Array *a_index = NULL, *a_deleted = NULL;
    char input[512], *token = NULL, *key = NULL;
    int ret = 0, strategy;
    char *database = NULL, *index = NULL, *deleted = NULL;
    
    if(argc <3){
        printf("Missing argument\n");
        return 0;
    }else{
        if (strcmp(argv[1], "best_fit") != 0 && strcmp(argv[1], "first_fit") != 0  && strcmp(argv[1], "worst_fit") != 0){
            printf("Unknown search strategy unknown_search_strategy\n");
            return 0;
        }

        if(strcmp("best_fit", argv[1]) == 0){
            strategy = BESTFIT;
        }else if(strcmp("worst_fit", argv[1]) == 0){                
            strategy = WORSTFIT;
        }else{
            strategy = FIRSTFIT;
        }

        a_index = (Array*)malloc(sizeof(Array));
        if(!a_index){
            fclose(db);
            free(database);
            free(index);
            fclose(ind);
            return 0;
        }
        initArray(a_index, 2);

        index = (char*)malloc(strlen(argv[2])+strlen(".ind")+1*sizeof(char));
        if(!index){
            free(database);
            return 0;
        }

        strcpy(index, argv[2]);
        strcat(index, ".ind");
        
        if((ind = fopen(index, "r"))!= NULL){
            Loadfromfile(ind, a_index, INDEX);
        }

        ind = fopen(index, "wb");
        if(!ind){
            fclose(db);
            free(database);
            free(index);
            return 0;
        }

        a_deleted = (Array*)malloc(sizeof(Array));
        if(!a_deleted){
            fclose(db);
            free(database);
            free(index);
            fclose(ind);
            return 0;
        }
        initArray(a_deleted, 2);

        deleted = (char*)malloc(strlen(argv[2])+strlen(".lst")+1*sizeof(char));
        if(!deleted){
            free(database);
            return 0;
        }

        strcpy(deleted, argv[2]);
        strcat(deleted, ".lst");
        
        if((del = fopen(deleted, "r"))!= NULL){
            Loadfromfile(del, a_deleted, DELETED);
        }

        del = fopen(deleted, "wb");
        if(!del){
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
                ret = add(db, ind, token, a_index);

                if(ret == -1){
                    printf("Error storing the data\n");
                    Exit(db, ind, del, database, index, deleted, a_index, a_deleted, strategy);
                    return 1;
                }else if(ret == 0){
                    printf("The key has already been introduced\n");
                }else{
                    printf("Record with BookID=%d has been added to the database\n", ret);
                    printf("exit\n");
                }
            }else if(strcmp("find", key)== 0){
                ret = find(database, a_index, atoi(token), 0, a_index->used);
                if(ret == -1){
                    printf("Record with bookId=%s does not exist\n", token);
                    Exit(db, ind, del, database, index, deleted, a_index, a_deleted, strategy);
                    return 1;
                }
            }else if(strcmp("del", key)== 0){
                ret = Del(a_index, a_deleted, atoi(token), strategy);
                if(ret == -1){
                    printf("Error storing the data\n");
                    Exit(db, ind, del, database, index, deleted, a_index, a_deleted, strategy);
                    return 1;
                }else if(ret == 0){
                    printf("Item with key %d does not exist\n", atoi(token));
                    printf("exit\n");
                }else{
                    printf("Record with BookID=%d has been deleted\n", ret);
                    printf("exit\n");
                }
            }else if(strcmp("exit\n", key)== 0){
                Exit(db, ind, del, database, index, deleted, a_index, a_deleted, strategy);
                break;
            }else if(strcmp("printRec\n", key)== 0){
                PrintRec(database, a_index);
            }else if(strcmp("printInd\n", key)== 0){
                printInd(a_index);
                printf("exit\n");
            }else if(strcmp("printLst\n", key)== 0){
                printLst(a_deleted);
                printf("exit\n");
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
    Indexbook *ibook = NULL;
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

    for(i=0; i<a->used; i++){
        ibook = (Indexbook*)a->array[i];
        if(ibook->key == book->id){
            return 0;
        }
    }
    ibook = NULL;

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

    ibook = (Indexbook*)malloc(sizeof(Indexbook));
    if(!ibook){
        return -1;
    }

    ibook->key = book->id;

    fseek(db, 0L, SEEK_END);
    
    ibook->offset = ftell(db)-(book->tamanio+8);
    ibook->size = book->tamanio;
    /*insertar indexbook a array*/
    insertArray(a, ibook, INDEX, NO_STRATEGY);

    /*hexdump -c archivo.bin*/

    free(book->isbn);
    free(book->titulo);
    free(book->editorial);
    free(book);

    if(ret != 1) return -1;
    
    return ibook->key;
}

int find(char *database, Array *a, int key, int ip, int iu){
    int m;
    long unsigned int i;
    Book *b = NULL;
    size_t ret, length;
    Indexbook *index = NULL;
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
        index = (Indexbook*)a->array[m];
        if(index->key == key){
            break;
        }else if(key < index->key){
            iu = m-1;
        }else{
            ip = m+1;
        }
    }

    if(index->key != key){
        fclose(db);
        free(b);
        return -1;
    }
    
    fseek(db, index->offset+sizeof(size_t), SEEK_SET);
    
    b->id = 0;
    ret = fread(&b->id, sizeof(int), 1, db);

    b->isbn = (char*)malloc(ISBN_LEN+1*sizeof(char));

    for(i = 0; i<ISBN_LEN+1; i++){
        b->isbn[i] = '\0';
    }
    ret = fread(b->isbn, ISBN_LEN, 1, db);

    ret = fread(buffer, index->size-sizeof(int)-ISBN_LEN, 1, db);

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

/* 
 * Borra el registro del fichero de datos y actualiza tanto el índice como la
 * lista de registros borrados.
 */
int Del(Array *a_index, Array *a_deleted, int key, int strategy){
    int ip = 0, iu = a_index->used;
    unsigned long int m;
    Indexbook *index = NULL;
    Indexdeletedbook *deleted = NULL;

    deleted = (Indexdeletedbook*)malloc(sizeof(Indexdeletedbook));
    if(!deleted) return -1;

    while(ip<=iu){
        m = (ip+iu)/2;
        index = (Indexbook*)a_index->array[m];
        if(index->key == key){
            break;
        }else if(key < index->key){
            iu = m-1;
        }else{
            ip = m+1;
        }
    }

    if(index->key != key){
        return 0; /*no encontrada*/
    }

    deleted->offset = index->offset;
    deleted->register_size = index->size;

    insertArray(a_deleted, deleted, DELETED, strategy);

    free(a_index->array[m]);
    while(m<a_index->used-1){
        a_index->array[m] = a_index->array[m+1];
        m++;
    }

    a_index->used--;

    return key;
}

void Exit(FILE *db, FILE *ind, FILE *del, char *database, char *index, char *deleted, Array *a_index, Array *a_deleted, int strategy){
    long unsigned int i;
    Saveinfile(ind, a_index, INDEX, NO_STRATEGY);
    Saveinfile(del, a_deleted, DELETED, strategy);/*guardar la estrategia de borrado*/
    
    if(a_index->array != NULL){
        if(a_index->used != 0){
            for(i=0; i<a_index->used; i++){
                free(a_index->array[i]);
            }
        }
        
        free(a_index->array);
        free(a_index);
    }

    if(a_deleted->array != NULL){
        if(a_deleted->used != 0){
            for(i=0; i<a_deleted->used; i++){
                free(a_deleted->array[i]);
            }
        }
        
        free(a_deleted->array);
        free(a_deleted);
    }
    
    if(db != NULL){
        fclose(db);
    }
    
    if(ind != NULL){
        fclose(ind);
    }

    if(del != NULL){
        fclose(del);
    }
    
    if(database != NULL){
        free(database);
    }

    if(index != NULL){
        free(index);
    }

    if(deleted != NULL){
        free(deleted);
    }
}

void PrintRec(char *database, Array *a){
    Book *b = NULL;
    FILE *db = NULL;
    Indexbook *index = NULL;
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
        index = (Indexbook*)a->array[i];

        fseek(db, index->offset+sizeof(size_t), SEEK_SET);

        fread(&b->id, sizeof(int), 1, db);
        fread(b->isbn, ISBN_LEN, 1, db);
        fread(buffer, index->size-sizeof(int)-ISBN_LEN, 1, db);

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
    Indexbook *index = NULL;

    for(i=0; i<a->used; i++){
        index = (Indexbook*)a->array[i];

        printf("Entry #%d\n", i);
        printf("    key: #%d\n", index->key);
        printf("    offset: #%ld\n", index->offset);
        printf("    size: #%ld\n", index->size);
    }
}

void printLst(Array *a){
    unsigned i;
    Indexdeletedbook *deleted = NULL;

    for(i=0; i<a->used; i++){
        deleted = (Indexdeletedbook*)a->array[i];

        printf("Entry #%d\n", i);
        printf("    offset: #%ld\n", deleted->offset);
        printf("    size: #%ld\n", deleted->register_size);
    }
}

void Saveinfile(FILE *pf, Array *a, int file_type, int strategy){
    unsigned i;
    Indexbook *index = NULL;
    Indexdeletedbook *deleted = NULL;
    
    if(file_type == INDEX){
        for(i=0; i<a->used; i++){
            index = (Indexbook*)a->array[i];

            fwrite(&index->key, sizeof(int), 1, pf);
            fwrite(&index->offset, sizeof(long), 1, pf);
            fwrite(&index->size, sizeof(size_t), 1, pf);
        }
    }else{
        fwrite(&strategy, sizeof(int), 1, pf);

        for(i=0; i<a->used; i++){
            deleted = (Indexdeletedbook*)a->array[i];

            fwrite(&deleted->offset, sizeof(size_t), 1, pf);
            fwrite(&deleted->register_size, sizeof(size_t), 1, pf);
        }
    }
}

void Loadfromfile(FILE *pf, Array *a, int file_type){
    Indexbook *index = NULL;
    Indexdeletedbook *deleted = NULL;
    int strategy;
    size_t ret = 1;

    if(file_type == INDEX){
        while(TRUE){
            index = (Indexbook*)malloc(sizeof(Indexbook));
            if(!index) return;

            ret = fread(&index->key, sizeof(int), 1, pf);
            ret = fread(&index->offset, sizeof(long), 1, pf);
            ret = fread(&index->size, sizeof(size_t), 1, pf);

            if(ret == 0){
                break;
            }

            insertArray(a, index, INDEX, NO_STRATEGY);
        }
        free(index);
    }else{
        ret = fread(&strategy, sizeof(int), 1, pf);

        while(TRUE){
            deleted = (Indexdeletedbook*)malloc(sizeof(Indexdeletedbook));
            if(!deleted) return;

            ret = fread(&deleted->offset, sizeof(size_t), 1, pf);
            ret = fread(&deleted->register_size, sizeof(size_t), 1, pf);

            if(ret == 0){
                break;
            }

            insertArray(a, deleted, DELETED, strategy);
        }
        free(deleted);
    }
    
    fclose(pf);
}

void initArray(Array *a, size_t initialSize){
    long unsigned int i;
    a->array = (void**)malloc(initialSize*sizeof(void*));

    if(!a->array) return;

    a->used = 0;
    a->size = initialSize;

    for(i=0; i<a->size; i++){
        a->array[i] = NULL;
    }
}

void insertArray(Array *a, void *element, int array_type, int strategy){
    Indexbook *elem = NULL, *key = NULL;
    Indexdeletedbook *e = NULL, *deleted = NULL;
    int j;
    if(a->used == a->size){
        a->size *=2;
        a->array = realloc(a->array, a->size*sizeof(void*));
    }

    a->array[a->used] = element;
    if(a->used > 0){
        if(array_type == INDEX){
            elem = (Indexbook*)a->array[a->used];
            j = a->used-1;

            key = (Indexbook*)a->array[j];

            while (j>= 0 && key->key > elem->key){
                key = (Indexbook*)a->array[j];
                a->array[j+1] = a->array[j];
                j--;
            }

            a->array[j+1] = elem;
        
        }else{
            if(strategy == BESTFIT){
                deleted = (Indexdeletedbook*)a->array[a->used];
                j = a->used-1;

                e = (Indexdeletedbook*)a->array[j];

                while (j>= 0 && e->register_size > deleted->register_size){
                    a->array[j+1] = a->array[j];
                    j--;
                    
                    if(j>=0){
                        e = (Indexdeletedbook*)a->array[j];
                    }
                }

                a->array[j+1] = deleted;
            }
            if(strategy == WORSTFIT){
                deleted = (Indexdeletedbook*)a->array[a->used];
                j = a->used-1;

                e = (Indexdeletedbook*)a->array[j];

                while (j>= 0 && e->register_size < deleted->register_size){
                    a->array[j+1] = a->array[j];
                    j--;

                    if(j>=0){
                        e = (Indexdeletedbook*)a->array[j];
                    }
                }

                a->array[j+1] = deleted;
            }
        }
    }
    
    a->used++;
}

void freeArray(Array *a){
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}