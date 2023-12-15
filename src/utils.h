#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include "types.h"

typedef struct _Indexbook{
    int key ;           /*!< key of the book */
    long int offset ;   /*!< offset of the register */
    size_t size;        /*!< size of the register */
}Indexbook;

typedef struct _Indexdeletedbook{
    size_t register_size;  /*!< size of the deleted register */
    size_t offset;         /*!< offset of the deleted register */
}Indexdeletedbook;

#define NO_STRATEGY -1
#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2

#define INDEX 0
#define DELETED 1

/**
  * @brief Añade una nueva entrada para un libro
  * 
  * @param db un puntero al fichero de la base de datos en la que vamos a almacenar el registro
  * @param arguments puntero a los datos que vamos a insertar en el registro
  * @param a_index un puntero al Array de indices para comprobar que el registro que se va a introducir no esta ya introducido
  * @return -1 si ha habido algun error durante la ejecucion, 0 si el id del libro ya ha sido introducido o book_id si todo ha salido correctamente
  */
int add(FILE *db, char *arguments, Array *a_index);

/**
  * @brief Busca un libro en la base de datos de libros
  * 
  * @param db un puntero al fichero de la base de datos en la que vamos a buscar el libro
  * @param a puntero al array de indices sobre el que vamos a buscar el id del libro
  * @param key el id del libro a buscar
  * @param ip el indice al primer elemento del array de indices (para realizar la Busqueda Binaria)
  * @param iu el indice al ultimo elemento del array de indices (para realizar la Busqueda Binaria)
  * @return -1 si ha habido algun error durante la ejecucion, 0 si el id del libro no esta en la base de datos y 1 si sale todo bien. Si todo sale correctamente se imprimira el registro completo a buscar
  */
int find(FILE *db, Array *a, int key, int ip, int iu);

/**
  * @brief Borra un registro de la base de datos de libros
  * 
  * @param a_index puntero al array de indices, para actualizarlo despues de borrar
  * @param a_deleted puntero al array de registros borrados, para actualizarlo despues de borrar
  * @param key el id del libro a borrar
  * @param strategy la estrategia (Best_fit, Worst_fit o First_fit) para el borrado
  * @return -1 si ha habido algun error durante la ejecucion, 0 si el id del libro no esta en la base de datos y la key del libro borrado si sale todo bien.
  */
int Del(Array *a_index, Array *a_deleted, int key, int strategy);

/**
  * @brief Libera toda la memoria usada por el programa, guarda los arrays de indices y de registros borrados
  *        en memoria y cierra todos los ficheros abiertos
  * 
  * @param db, ind, del punteros a los ficheros abiertos
  * @param database, index, deleted punteros a variables de tipo char a las que se ha reservado memoria para la extension de los ficheros
  * @param a_index, a_deleted Arrays de indices y de registros borrados
  * @param strategy la estrategia (Best_fit, Worst_fit o First_fit) para guardar en el fichero de registros borrados
  * @return no tiene retorno
  */
void Exit(FILE *db, FILE *ind, FILE *del, char *database, char *index, char *deleted, Array *a_index, Array *a_deleted, int strategy);

/**
  * @brief Imprime todos los registros almacenados en la base de datos
  * 
  * @param db un puntero al fichero de la base de datos
  * @param a puntero al array de indices para saber cuales, cuantos y donde se encuentran los registros
  * @return -1 si ha habido algun error durante la ejecucion, 0 si todo ha salido correctamente
  */
int PrintRec(FILE *db, Array *a);

/**
  * @brief Imprime el listado de indices
  * 
  * @param a puntero al array de indices para imprimir
  * @return no tiene retorno
  */
void printInd(Array *a);

/**
  * @brief Imprime el listado de registros borrados
  * 
  * @param a puntero al array de registros borrados para imprimir
  * @return no tiene retorno
  */
void printLst(Array *a);

/**
  * @brief Guarda en un fichero los datos del array que se pasa como parametro dependiendo de que tipo de fichero o que estrategia
  *        se hace una cosa u otra
  * 
  * @param pf puntero al fichero en el que se va a guardar los datos
  * @param a puntero a un array generico del cual se van a sacar los datos para guardarlos
  * @param file_type tipo de fichero en el que se van a guardar los datos (fichero de indices o fichero de registros borrados)
  * @param strategy estrategia de borrado que se va a seguir (en el caso de que nos encontremos ante un fichero de registros borrados)
  * @return ERROR si ha habido algun error durante la ejecucion, OK si todo ha salido correctamente
  */
STATUS Saveinfile(FILE *pf, Array *a, int file_type, int strategy);

/**
  * @brief Carga un array mediante el contenido de un fichero de datos binario (siempre que exista)
  * 
  * @param pf puntero al fichero en el que se van a sacar los datos
  * @param a puntero a un array generico el cual se va a llenar con los datos del fichero
  * @param file_type tipo de fichero en el que se van a guardar los datos (fichero de indices o fichero de registros borrados)
  * @return ERROR si ha habido algun error durante la ejecucion, OK si todo ha salido correctamente
  */
STATUS Loadfromfile(FILE *pf, Array *a, int file_type);

#endif