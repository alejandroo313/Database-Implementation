#include "utils.h"
#include "array.h"
#include <string.h>

/** Bucle principal del programa. Mientras tenga un input
 *  del usuario va a estar leyendo la entrada. Dependiendo de 
 *  lo que se pase por teclado hara una funcion u otra
 */
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

        /* Control de la estrategia de borrado */
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
            return 1;
        }
        initArray(a_index, 2);

        index = (char*)malloc(strlen(argv[2])+strlen(".ind")+1*sizeof(char));
        if(!index){
            freeArray(a_index);
            return 1;
        }

        strcpy(index, argv[2]);
        strcat(index, ".ind");
        
        if((ind = fopen(index, "r"))!= NULL){
            if((Loadfromfile(ind, a_index, INDEX))==ERROR){
                printf("Error cargando los datos del indice\n");
                freeArray(a_index);
                free(index);
                fclose(ind);
                return 1;
            }
        }

        ind = fopen(index, "wb");
        if(!ind){
            freeArray(a_index);
            free(index);
            return 1;
        }

        a_deleted = (Array*)malloc(sizeof(Array));
        if(!a_deleted){
            freeArray(a_index);
            free(index);
            fclose(ind);
            return 1;
        }
        initArray(a_deleted, 2);

        deleted = (char*)malloc(strlen(argv[2])+strlen(".lst")+1*sizeof(char));
        if(!deleted){
            freeArray(a_index);
            free(index);
            fclose(ind);
            freeArray(a_deleted);
            return 1;
        }

        strcpy(deleted, argv[2]);
        strcat(deleted, ".lst");
        
        if((del = fopen(deleted, "r"))!= NULL){
            if((Loadfromfile(del, a_deleted, DELETED))==ERROR){
                printf("Error cargando los datos de los registros borrados\n");
                freeArray(a_index);
                free(index);
                fclose(ind);
                fclose(del);
                free(deleted);
                freeArray(a_deleted);
                return 1;
            }
        }

        del = fopen(deleted, "wb");
        if(!del){
            freeArray(a_index);
            free(index);
            fclose(ind);
            freeArray(a_deleted);
            return 1;
        }

        database = (char*)malloc(strlen(argv[2])+strlen(".db")+1*sizeof(char));
        if(!database){
            freeArray(a_index);
            free(index);
            fclose(ind);
            fclose(del);
            freeArray(a_deleted);
            return 1;
        }

        strcpy(database, argv[2]);
        strcat(database, ".db");

        if(db == NULL){
            db = fopen(database, "wb+");
            if(!db){
                free(database);
                free(index);
                return 0;
            }
        }

        printf("Type command and argument/s.\n");
        printf("exit\n");
        while (fgets(input, sizeof(input), stdin)) {
            
            token = strtok(input, " ");
            key = token;
            token = strtok(NULL, "");

            if(strcmp("add", key) == 0){
                ret = add(db, token, a_index, a_deleted);

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

                ret = find(db, a_index, atoi(token), 0, a_index->used);

                if(ret == -1){
                    printf("Error searching the book\n");
                    Exit(db, ind, del, database, index, deleted, a_index, a_deleted, strategy);
                    return 1;
                }else if(ret == 0){
                    printf("Record with bookId=%d does not exist\n", atoi(token));
                    Exit(db, ind, del, database, index, deleted, a_index, a_deleted, strategy);
                }

            }else if(strcmp("del", key)== 0){

                ret = Del(a_index, a_deleted, db, atoi(token), strategy);

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
                return 0;
            }else if(strcmp("printRec\n", key)== 0){

                ret = PrintRec(db, a_index);

                if(ret == -1){
                    printf("Error imprimiendo los registros\n");
                    Exit(db, ind, del, database, index, deleted, a_index, a_deleted, strategy);
                    return 1;
                }

                printf("exit\n");

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