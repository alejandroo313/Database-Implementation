#include "utils.h"
#include <string.h>

#define ISBN_LEN 16

/**
 * @brief book
 *
 * Esta estructura almacena los diferentes campos del registro Libro
 */
typedef struct _Book{
    size_t tamanio;     /*!< Tamaño del registro */
    int id;             /*!< Identificador unico del libro */
    char *isbn;         /*!< ISBN del libro */
    char *titulo;       /*!< Titulo del libro */
    char *editorial;    /*!<Editorial */
}Book;

/** add reserva memoria para una entrada del array de indices
  *  y lo inicializa
  */
int add(FILE *db, char *arguments, Array *a_index, Array *a_deleted){
    char *token = NULL;
    Book *book = NULL;
    Indexbook *ibook = NULL;
    Indexdeletedbook *deleted = NULL;
    int ret = 0;
    size_t length = 0;
    long unsigned int i;
    
    /* Control de Errores */
    if(!db || !arguments || !a_index) return -1;

    fseek(db, 0L, SEEK_END);

    book = (Book*)malloc(sizeof(Book));
    if(!book) return -1;

    /* sacamos book_id de arguments y lo copiamos a la estructura */
    token = strtok(arguments, "|");
    if(!token){
        free(book);
        return -1;
    }
    
    book->id = atoi(token);

    /* si el id ya se encuentra en el array de indices, no se inserta */
    for(i=0; i<a_index->used; i++){
        ibook = (Indexbook*)a_index->array[i];
        if(ibook->key == book->id){
            return 0;
        }
    }
    ibook = NULL;

    /* sacamos el isbn de arguments y lo copiamos a la estructura */
    token = strtok(NULL, "|");
    if(!token){
        free(book);
        return -1;
    }

    book->isbn = (char*)malloc(ISBN_LEN+1*sizeof(char));
    if(!book->isbn){
        free(book);
        return -1;
    }

    for(i = 0; i<ISBN_LEN+1; i++){
        book->isbn[i] = '\0';
    }

    strncpy(book->isbn, token, ISBN_LEN);

    /* sacamos el titulo de arguments, reservamos memoria para
     * el titulo y lo copiamos a la estructura
     */
    token = strtok(NULL, "|");
    if(!token){
        free(book->isbn);
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

    /* sacamos la editorial de arguments, reservamos memoria para
     * la editorial y lo copiamos a la estructura
     */
    token = strtok(NULL, "\n");
    if(!token){
        free(book->isbn);
        free(book->titulo);
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
    
    for(i=0; i<a_deleted->used; i++){
        deleted = (Indexdeletedbook*) a_deleted->array[i];
        if(deleted->register_size >= book->tamanio){
            fseek(db, deleted->offset, SEEK_SET);

            /* se borra la entrada y se mueven todas una posicion a la izquierda */
            free(a_deleted->array[i]);
            while(i<a_deleted->used-1){
                a_deleted->array[i] = a_deleted->array[i+1];
                i++;
            }

            a_deleted->used--;
        }
    }

    /*copiamos toda la informacion en el fichero binario*/
    ret = fwrite(&book->tamanio, sizeof(size_t), 1, db);
    ret = fwrite(&book->id, sizeof(int), 1, db);
    ret = fwrite(book->isbn, ISBN_LEN, 1, db);
    ret = fwrite(book->titulo, strlen(book->titulo), 1, db);
    ret = fwrite(book->editorial, length, 1, db);

    if(ret != 1){
        free(book->isbn);
        free(book->titulo);
        free(book->editorial);
        free(book);
        return -1;
    } 

    /* generarmos un indexbook */
    ibook = (Indexbook*)malloc(sizeof(Indexbook));
    if(!ibook){
        free(book->isbn);
        free(book->titulo);
        free(book->editorial);
        free(book);
        return -1;
    }

    ibook->key = book->id;
    
    ibook->offset = ftell(db)-(book->tamanio+8);
    ibook->size = book->tamanio;

    /* insertamos indexbook a array */
    if((insertArray(a_index, ibook, INDEX, NO_STRATEGY) == ERROR)){
        free(book->isbn);
        free(book->titulo);
        free(book->editorial);
        free(book);
        free(ibook);
        return -1;
    }

    free(book->isbn);
    free(book->titulo);
    free(book->editorial);
    free(book);

    return ibook->key;
}


/** find busca mediante la Busqueda Binaria un libro
  *  si lo encuentra, se obtienen sus parametros
  *  del fichero de base de datos y se imprimen en pantalla
  */
int find(FILE *db, Array *a, int key, int ip, int iu){
    int m;
    long unsigned int i;
    Book *b = NULL;
    size_t ret, length;
    Indexbook *index = NULL;
    char buffer[256] = {'\0'}, *token = NULL;

    /* Control de Errores */
    if(!db || !a || key<0 || ip>iu) return -1;

    b = (Book*)malloc(sizeof(Book));
    if(!b) return -1;

    /* Busqueda Binaria sobre la clave */
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
        free(b);
        return 0;
    }
    
    /* Apuntar al fichero que hemos encontrado */
    fseek(db, index->offset+sizeof(size_t), SEEK_SET);
    
    b->id = 0;
    ret = fread(&b->id, sizeof(int), 1, db);

    if(ret != 1){
        free(b);
        return -1;
    }

    b->isbn = (char*)malloc(ISBN_LEN+1*sizeof(char));
    if(!b->isbn){
        free(b);
        return -1;
    }

    for(i = 0; i<ISBN_LEN+1; i++){
        b->isbn[i] = '\0';
    }

    ret = fread(b->isbn, ISBN_LEN, 1, db);

    /* Usamos un buffer para sacar el titulo y la editorial del fichero */
    ret = fread(buffer, index->size-sizeof(int)-ISBN_LEN, 1, db);

    if(ret != 1){
        free(b->isbn);
        free(b);
        return -1;
    }
    
    token = strtok(buffer, "|");
    length = strlen(token);

    b->titulo = (char*)malloc(length+1*sizeof(char));
    if(!b->titulo){
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
        free(b->isbn);
        free(b->titulo);
        free(b);
        return -1;
    }

    for(i = 0; i<length+1; i++){
        b->editorial[i] = '\0';
    }

    strncpy(b->editorial, token, length);

    /* Imprimimos el resultado */
    printf("%d|%s|%s|%s\n", b->id, b->isbn, b->titulo, b->editorial);

    free(b->isbn);
    free(b->titulo);
    free(b->editorial);
    free(b);

    return 1;
}

/* 
 * Borra el registro del fichero de datos y actualiza tanto el índice como la
 * lista de registros borrados.
 */
int Del(Array *a_index, Array *a_deleted, FILE *db, int key, int strategy){
    int ip = 0, iu;
    unsigned long int m;
    Indexbook *index = NULL;
    Indexdeletedbook *deleted = NULL;
    char buffer[255] = {'\0'};

    if(!a_index || !a_deleted || key<0 || strategy<-1) return -1;

    iu = a_index->used;

    /* se realiza la Busqueda Binaria sobre la clave*/
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

    /* clave no encontrada */
    if(index->key != key){
        return 0; 
    }

    /* se reserva memoria para una nueva entrada al array de registros borrados*/
    deleted = (Indexdeletedbook*)malloc(sizeof(Indexdeletedbook));
    if(!deleted) return -1;

    deleted->offset = index->offset;
    deleted->register_size = index->size;

    fseek(db, deleted->offset, SEEK_SET);
    
    if((fwrite(buffer, deleted->register_size, 1, db)) != 1){
        free(deleted);
        return -1;
    }

    if((insertArray(a_deleted, deleted, DELETED, strategy)) == ERROR){
        free(deleted);
        return -1;
    }

    /* se borra la entrada y se mueven todas una posicion a la izquierda */
    free(a_index->array[m]);
    while(m<a_index->used-1){
        a_index->array[m] = a_index->array[m+1];
        m++;
    }

    a_index->used--;

    return key;
}

/* 
 * Libera todos los recursos utilizados por el programa
 */
void Exit(FILE *db, FILE *ind, FILE *del, char *database, char *index, char *deleted, Array *a_index, Array *a_deleted, int strategy){
    

    if((Saveinfile(ind, a_index, INDEX, NO_STRATEGY)) == ERROR) return;

    /*guardar la estrategia de borrado*/
    if((Saveinfile(del, a_deleted, DELETED, strategy)) == ERROR) return;
    
    if(a_index->array != NULL){
        freeArray(a_index);
    }

    if(a_deleted->array != NULL){
        freeArray(a_deleted);
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

/* 
 * Imprime todos los registros guardados en la base de datos
 */
int PrintRec(FILE *db, Array *a){
    Book *b = NULL;
    Indexbook *index = NULL;
    size_t length, ret;
    long unsigned int i, j;
    char buffer[256] = {'\0'};

    if(!db || !a) return -1;

    b = (Book*)malloc(sizeof(Book));
    if(!b){
        return -1;
    } 

    b->isbn = (char*)malloc(ISBN_LEN+1*sizeof(char));
    if(!b->isbn){
        free(b);
        return -1;
    }

    for(i = 0; i<ISBN_LEN+1; i++){
        b->isbn[i] = '\0';
    }

    for(i=0; i<a->used; i++){
        index = (Indexbook*)a->array[i];

        fseek(db, index->offset+sizeof(size_t), SEEK_SET);

        ret = fread(&b->id, sizeof(int), 1, db);
        ret = fread(b->isbn, ISBN_LEN, 1, db);
        ret = fread(buffer, index->size-sizeof(int)-ISBN_LEN, 1, db);

        if(ret != 1){
            free(b->isbn);
            free(b);
            return -1;
        }

        printf("%d|%s|%s\n", b->id, b->isbn, buffer);

        length = strlen(buffer);
        for(j=0; j<length+1; j++){
            buffer[j] = '\0';
        }
    }
    
    free(b->isbn);
    free(b);

    return 0;
}

/* 
 * Imprime los indices de los registros
 */
void printInd(Array *a){
    unsigned i;
    Indexbook *index = NULL;

    if(!a) return;

    for(i=0; i<a->used; i++){
        index = (Indexbook*)a->array[i];

        printf("Entry #%d\n", i);
        printf("    key: #%d\n", index->key);
        printf("    offset: #%ld\n", index->offset);
        printf("    size: #%ld\n", index->size);
    }
}

/* 
 * Imprime la informacion de los registros borrados
 */
void printLst(Array *a){
    unsigned i;
    Indexdeletedbook *deleted = NULL;

    if(!a) return;

    for(i=0; i<a->used; i++){
        deleted = (Indexdeletedbook*)a->array[i];

        printf("Entry #%d\n", i);
        printf("    offset: #%ld\n", deleted->offset);
        printf("    size: #%ld\n", deleted->register_size);
    }
}


/* 
 * Guarda en un fichero los datos del array
 */
STATUS Saveinfile(FILE *pf, Array *a, int file_type, int strategy){
    unsigned i;
    Indexbook *index = NULL;
    Indexdeletedbook *deleted = NULL;
    size_t ret = 0;

    if(!pf || !a || file_type<0 || strategy<-1) return ERROR;
    
    if(file_type == INDEX){
        for(i=0; i<a->used; i++){
            index = (Indexbook*)a->array[i];

            ret = fwrite(&index->key, sizeof(int), 1, pf);
            ret = fwrite(&index->offset, sizeof(long), 1, pf);
            ret = fwrite(&index->size, sizeof(size_t), 1, pf);

            if(ret != 1){
                return ERROR;
            }
        }
    }else{
        ret = fwrite(&strategy, sizeof(int), 1, pf);

        for(i=0; i<a->used; i++){
            deleted = (Indexdeletedbook*)a->array[i];

            ret = fwrite(&deleted->offset, sizeof(size_t), 1, pf);
            ret = fwrite(&deleted->register_size, sizeof(size_t), 1, pf);

            if(ret != 1){
                return ERROR;
            }
        }
    }

    return OK;
}

/* 
 * Carga un array mediante un fichero de datos
 */
STATUS Loadfromfile(FILE *pf, Array *a, int file_type){
    Indexbook *index = NULL;
    Indexdeletedbook *deleted = NULL;
    int strategy;
    size_t ret = 1;

    if(!pf || !a || file_type<0){
        return ERROR;
    }

    if(file_type == INDEX){
        while(TRUE){
            index = (Indexbook*)malloc(sizeof(Indexbook));
            if(!index) return ERROR;

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

        if(ret != 1){
            return ERROR;
        }

        while(TRUE){
            deleted = (Indexdeletedbook*)malloc(sizeof(Indexdeletedbook));
            if(!deleted) return ERROR;

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

    return OK;
}
