#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*switch key--------terminal key*/

int main( int argc, char *argv[]){
    FILE *pf = NULL;
    char key[10] = {'\0'};
    
    if(argc <3){
        printf("Error, the program needs 2 param in form:\n");
        printf("./<file> <sort_estrategy> <data_file_names>\n");
        return 1;
    }else{
        printf("Enter the operation you want to do: ");
        while (scanf("%s", key) != EOF) {
            if(strcmp("add", key) == 0){
                printf("add\n");
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
        printf("Enter the operation you want to do: ");
        }
    return 0;
    }
    
    
}
