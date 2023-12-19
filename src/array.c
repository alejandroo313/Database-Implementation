#include "array.h"

/** initArray reserva memoria e inicializa una estructura de 
  *  tipo array 
  */
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

/** insertArray inserta un elemento generico al array de punteros
  * si el array esta lleno se incrementa su capacidad
  * dependiendo del tipo de array y la estrategia se insertara 
  * de una forma u ortra
  */
STATUS insertArray(Array *a, void *element, int array_type, int strategy){
    Indexbook *elem = NULL, *key = NULL;
    Indexdeletedbook *e = NULL, *deleted = NULL;
    int j;

    /* Control de Errores*/
    if(!a || !element || array_type<0 || strategy<-1){
        return ERROR;
    }

    /* Se expande el array si esta lleno */
    if(a->used == a->size){
        a->size *=2;
        a->array = realloc(a->array, a->size*sizeof(void*));

        if(a->array == NULL){
            return ERROR;
        }
    }

    a->array[a->used] = element;

    /* Si el array tiene mas de un elemento */
    if(a->used > 0){

        /* Si el array es de tipo indices */
        if(array_type == INDEX){
            elem = (Indexbook*)a->array[a->used];
            j = a->used-1;

            key = (Indexbook*)a->array[j];

            while (j>= 0 && key->key > elem->key){
                a->array[j+1] = a->array[j];
                j--;

                if(j>=0){
                    key = (Indexbook*)a->array[j];
                }
            }

            a->array[j+1] = elem;
        /* Si el array es de tipo registros borrados */
        }else{

            /* Si la estrategia es best_fit */
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

            /* Si la estrategia es worst_fit */
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

            /* Si la estrategia es first_fit simplemente se añade la entrada al final */
        }
    }
    
    a->used++;

    return OK;
}

/* 
 * Libera todos los recursos utilizados por un array
 */
void freeArray(Array *a){
    long unsigned int i;

    if(a->used != 0){
        for(i=0; i<a->used; i++){
            free(a->array[i]);
        }
    }

    free(a->array);
    free(a);
}