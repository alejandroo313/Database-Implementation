#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    size_t tamanio;
    int id;
    char isbn[16];
    char *titulo;
    char *editorial;
}books;


int main(char argc, char*argv[]){
    char funcion[] = {"add 12345|978-2-12345680-3|El Quijote|catedra"};
    books book;
    char hola[] = {"database"}; 
    char *token = NULL;
    char *key = NULL;
    char *input = NULL;

    printf("funcion = %s\n", funcion);
    token = strtok(funcion, " ");

    key = (char*)malloc(strlen(token)*sizeof(char));
    if(key == NULL) return 1;

    strcpy(key, token);
    printf("key = %s\n", key);

    token = strtok(NULL, "");
    strcpy(funcion, token);
    printf("funcion = %s\n", funcion);
    
    token = strtok(funcion, "|");
    book.id = atoi(token);

    token = strtok(NULL, "|");
    strcpy(book.isbn, token);

    token = strtok(NULL, "|");
    book.titulo = (char*)malloc(strlen(token)*sizeof(char));
    if(!book.titulo) return 1;
    strcpy(book.titulo, token);
    strcat(book.titulo, "|");

    token = strtok(NULL, "|");
    book.editorial = (char*)malloc(strlen(token)*sizeof(char));
    if(!book.editorial) return 1;
    strcpy(book.editorial, token);

    book.tamanio = strlen(book.isbn) + strlen(book.titulo) + strlen(book.editorial) + 4;

    printf("ID = %d\n", book.id);
    printf("ISBN = %s\n", book.isbn);
    printf("title = %s\n", book.titulo);
    printf("editorial = %s\n", book.editorial);
    printf("tamanio = %ld\n", book.tamanio);

    printf("int: %ld, long: %ld", sizeof(int), sizeof(long int));

    input = (char*)malloc(book.tamanio*sizeof(char));
    if(!input){
        free(book.titulo);
        free(book.editorial);
        return 1;
    }

    sprintf(input, "%ld %d %s %s %s\n", book.tamanio, book.id, book.isbn, book.titulo, book.editorial);

    printf("%s", input);
    printf("\\");
    hola[8] = '\0';
    printf("%ld, %ld", sizeof(argv[1]), strlen(argv[1])+strlen(".ind")+1);
    free(key);
    free(book.titulo);
    free(book.editorial);
    return 0;
}