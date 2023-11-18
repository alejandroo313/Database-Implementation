#include "library.h"
#include <string.h>
/*switch key--------terminal key*/

#define ISBN_LEN 16

struct _Book{
    size_t tamanio;
    int id;
    char isbn[ISBN_LEN];
    char *titulo;
    char *editorial;
};

int main( int argc, char *argv[]){
    FILE *pf = NULL;
    char input[512];
    char *token = NULL;
    char *key = NULL;
    char *funcion = NULL;
    int ret = 0;
    
    if(argc <3){
        printf("Error, the program needs 2 param in form:\n");
        printf("./<file> <sort_estrategy> <data_file_names>\n");
        return 1;
    }else{
        if (strcmp(argv[1], "best_fit") != 0 && strcmp(argv[1], "first_fit") != 0  && strcmp(argv[1], "worst_fit") != 0){
            printf("Wrong strategy, error\n");
            return 1;
        }

        strcat(argv[2], ".db");
        pf = fopen(argv[2], "wb");
        if(!pf){
            return 1;
        }

        printf("Type command and argument/s: ");
        while (fgets(input, sizeof(input), stdin)) {

            /*obtienes la palabra clave del comando*/
            token = strtok(input, " ");

            key = (char*)malloc(strlen(token)*sizeof(char));
            if(key == NULL) return 1;

            strcpy(key, token);

            /*guardas lo que queda (datos, si hay) para pasarlo a la funcion*/
            token = strtok(NULL, "");
            if(token != NULL){
                funcion = (char*)malloc(strlen(token)*sizeof(char));
                if(funcion == NULL) return 1;
                strcpy(funcion, token);
            }
            
            if(strcmp("add", key) == 0){
                ret = add(pf, funcion);
                if(ret == -1){
                    printf("Error en el guardado de los datos\n");
                    return 1;
                }
                printf("Record with BookID=%d has been added to the database\n", ret);
            }else if(strcmp("find", key)== 0){
                printf("find\n");
            }else if(strcmp("del", key)== 0){
                printf("del\n");
            }else if(strcmp("exit", key)== 0){
                printf("exit\n");
            }else if(strcmp("printRec", key)== 0){
                printf("printRec\n");
            }else if(strcmp("printInd", key)== 0){
                printf("printInd\n");
            }else if(strcmp("printLst", key)== 0){
                printf("printLst\n");
            }else{
                printf("Error, that operation doesn't exists\n");
            }
        printf("Type command and argument/s: ");
        }
    return 0;
    }
}

/* 
 * Funcion que añade el registro a la base de datos
 * Si no hay fallos en la ejecucion, la funcion devuelve el BookId
 * Si hay algun error en la ejecucion, la funcion devuelve -1
*/
int add(FILE *pf, char *arguments){
    /*hacer la particion del contenido de arguments y guardarlo en books propia de la funcion*/
    /*escribir en el fichero pf en binario los datos y hacer el retorno oportuno*/
    char *token = NULL;
    Book book;
    char *input = NULL;
    int ret = 0;
    
    if(!pf || !arguments) return -1;

    /*sacamos book_id de arguments y lo copiamos a la estructura*/
    token = strtok(arguments, "|");
    book.id = atoi(token);

    /*sacamos el isbn de arguments y lo copiamos a la estructura*/
    token = strtok(NULL, "|");
    strcpy(book.isbn, token);

    /*sacamos el titulo de arguments, reservamos memoria para
     *el titulo y lo copiamos a la estructura
     */
    token = strtok(NULL, "|");
    book.titulo = (char*)malloc(strlen(token)*sizeof(char));
    if(!book.titulo) return -1;
    strcpy(book.titulo, token);
    strcat(book.titulo, "|");

    /*sacamos la editorial de arguments, reservamos memoria para
     *la editorial y lo copiamos a la estructura
     */
    token = strtok(NULL, "\n");
    book.editorial = (char*)malloc(strlen(token)*sizeof(char));
    if(!book.editorial){
        free(book.titulo);
        return -1;
    }
    strcpy(book.editorial, token);

    /*obtenemos el tamaño total del registro*/
    book.tamanio = strlen(book.isbn) + strlen(book.titulo) + strlen(book.editorial) + 4;

    /*copiamos a un array la info que vamos a introducir en el fichero binario*/
    input = (char*)malloc((book.tamanio+sizeof(book.tamanio))*sizeof(char));
    if(!input){
        free(book.titulo);
        free(book.editorial);
        return 1;
    }
    sprintf(input, "%ld %d %s %s %s\n", book.tamanio, book.id, book.isbn, book.titulo, book.editorial);
    printf("ID = %d\n", book.id);
    printf("ISBN = %s\n", book.isbn);
    printf("title = %s\n", book.titulo);
    printf("editorial = %s\n", book.editorial);
    printf("tamanio = %ld\n", book.tamanio);

    /*PROBLEMA AL PASAR STRINGS*/    
    /*copiamos toda la informacion en el fichero binario*/
    ret = fwrite(&book, book.tamanio+sizeof(book.tamanio), 1, pf);
    if(ret != 1){
        free(book.titulo);
        free(book.editorial);
        free(input);
        return -1;
    }

    free(book.titulo);
    free(book.editorial);
    free(input);

    /*hexdump -c archivo.bin*/
    fclose(pf);
    
    return book.id;
}   

/*
 * 0026 0000 0000 0000 (38)
 * 3039 0000 (12345)
 * 3739 2d38 2d32 3231 3433 3635 3038 332d
 * 6c45 5120 6975 6f6a 6574 437c 7461 6465 6172 (El Quijote| Catedra)
*/