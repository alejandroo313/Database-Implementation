#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

typedef struct _Array{
    void **array;  /*!< array of pointers */
    size_t used;   /*!< number of positions ocupied */
    size_t size;   /*!< number of positions possible */
}Array;

#include "utils.h"

/**
  * @brief Realiza un swap entre dos valores
  * 
  * @param val1 puntero al primer valor que vamos a swappear
  * @param val2 puntero al segundo valor
  * @return no tiene retorno
  */
void swap(void *val1, void *val2);

/**
  * @brief Inicializa un array reservando memoria para las posiciones que indique initialSize
  * 
  * @param a puntero al array que vamos a inicializar
  * @param initialSize numero de posiciones iniciales que querenos que tenga el array
  * @return no tiene retorno
  */
void initArray(Array *a, size_t initialSize);

/**
  * @brief Inserta un elemento al array correspondiente
  * 
  * @param a puntero array donde se va a insertar el elemento
  * @param element elemento generico que se va a insertar al array
  * @param array_type tipo de array en el que se van a guardar los datos (array de indices o array de registros borrados)
  * @param strategy estrategia de borrado que se va a seguir (en el caso de que nos encontremos ante un array de registros borrados)
  * @return ERROR si ha habido algun error durante la ejecucion, OK si todo ha salido correctamente
  */
STATUS insertArray(Array *a, void *element, int array_type, int strategy);

/**
  * @brief Libera la memoria usada por el array (libera tambien la memoria a la que apunta el array de punteros)
  * 
  * @param a puntero al array que vamos a liberar
  * @return no tiene retorno
  */
void freeArray(Array *a);

#endif