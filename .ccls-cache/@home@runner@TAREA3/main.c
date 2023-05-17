#include "heap.h"
#include "list.h"
#include "map.h"
#include "stack.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nodo {
  void *data;
  int priority;
} heapElem;

typedef struct Heap {
  heapElem *heapArray;
  int size;
  int capac;
} Heap;

typedef struct { // Nodo que se guardara en el grafo
  char *nombre;
  int prioridad;
  bool completado;
  List *nodosAdj;
} Nodo;

typedef struct {    // stack de acciones realizadas para el deshacer acciones
  int numeroAccion; // que se realizo
  Nodo tarea;       // la tarea que se realizo
} accion;

int is_equal_string(void *key1, void *key2) {
  if (strcmp((char *)key1, (char *)key2) == 0)
    return 1;
  return 0;
}

int lower_than_prioridad(void *key1, void *key2) {
  Nodo *tarea1 = (Nodo *)key1;
  Nodo *tarea2 = (Nodo *)key2;
  if (tarea1->prioridad < tarea2->prioridad)
    return -1;
  return 0; 
}

void menuTexto(int *opcion) {

  printf("\nElija una opcion para continuar:\n");
  printf("\n1. Agregar tarea.\n");
  printf("2. establecer precedencia entre tareas.\n");
  printf("3. Mostrar tareas por hacer.\n");
  printf("4. Marcar tarea como completada.\n");
  printf("5. Deshacer última acción.\n");
  printf("6. Importar datos desde un archivo de texto.\n");
  printf("0. Salir del menu.\n");
  /*
    Usar string para comprobar que lo ingresa el usuario es un numero valido
    para nuestro menu
  */
  char comprobar[2];
  while (1) {
    scanf("%s", comprobar);

    if (isdigit(comprobar[0]) && strlen(comprobar) == 1)
    // ver que sea un numero de un digito
    {

      *opcion = atoi(comprobar);
      // ya cuando se comprueba se actualiza opcion
      return;
    }
    printf("Entrada invalida, ingrese una opcion valida entre 0 y 9\n");
  }
}

void agregarTarea(Map *grafo) {

  char nombre[50];
  int prioridad;
  Nodo *tarea = (Nodo *)malloc(sizeof(Nodo));
  tarea->nodosAdj = createList();
  tarea->completado = false;

  printf("Ingrese el nombre de la Tarea: ");
  scanf("%s", nombre);
  tarea->nombre = malloc(strlen(nombre) + 1);
  strcpy(tarea->nombre, nombre);

  printf("Ingrese la prioridad de la Tarea: ");
  scanf("%i", &prioridad);
  tarea->prioridad = prioridad;
  // Establecer la función de ordenamiento por prioridad
  insertMap(grafo, tarea->nombre, tarea);

  printf("\nTarea agregada con éxito.\n");
} // LISTO(?)

void establecerPrecedencia(Map *grafo) {
  char tarea1[50];
  char tarea2[50];

  if (firstMap(grafo) == NULL) {
    puts("\nNo hay tareas guardadas");
    return;
  }

  printf("Ingrese el nombre de la primera tarea: ");
  scanf("%s", tarea1);
  
    if (!searchMap(grafo, tarea1)) {

      puts("La tarea no se encuentra guardada.");
      return;
    }
    

  printf("\nIngrese el nombre de la segunda tarea: ");
  scanf("%s", tarea2);
   if (searchMap(grafo, tarea2) == NULL) {

      puts("\nLa tarea no se encuentra guardada.");
      return;
    }
  

  Nodo *t1 = searchMap(grafo, tarea1);
  Nodo *t2 = searchMap(grafo, tarea2);

  if (searchMap(grafo, tarea1) != NULL) {

    pushBack(t1->nodosAdj, t2);
  }

} // LISTO(?)

void mostrarTareas(Map *grafo) {
  Heap *heap = createHeap();
  Map *tareasMostradas = createMap(is_equal_string);

  // Agregar todas las tareas al heap
  Nodo *aux = firstMap(grafo);
  while (aux != NULL) {   // se ordena en un monticulo y se guardan los datos en
                          // un mapa auxiliar
    aux->prioridad *= -1; // para que se guarden de menor a mayor
    heap_push(heap, aux, aux->prioridad);
    aux = nextMap(grafo);
  }

  int contador = 1;
  printf("Tareas por hacer, ordenadas por prioridad y precedencia:\n");
  while (heap->size > 0) 
  {

    Nodo *tarea = (Nodo *)heap_top(heap); // se guarda el menor
    heap_pop(heap);

    if (!tarea->completado && searchMap(tareasMostradas, tarea->nombre) == NULL) 
    { // si la tarea no esta completa y aun no se muestra
      tarea->prioridad *=-1; // estaba en negativo, se pasa a positivo para mostrarlo

      printf("%d. %s (Prioridad: %d)\n", contador, tarea->nombre,
             tarea->prioridad);
      insertMap(tareasMostradas, tarea->nombre, tarea); // se actualiza

      Heap *nodosAdj =  createHeap(); // para almacenar los precedentes.
      Nodo *auxList = firstList(tarea->nodosAdj);
      while(auxList) // se llena el monticulo
        {
          heap_push(nodosAdj, auxList, auxList->prioridad);
          auxList = nextList(tarea->nodosAdj);
        }
      
      if (nodosAdj ->size > 0) 
      {
        Heap *tempHeap = createHeap(); // Montículo temporal para guardar los precedentes

          while (nodosAdj->size > 0) 
          {
            Nodo *precedente = (Nodo *)heap_top(nodosAdj);
            heap_pop(nodosAdj);
  
            if (!precedente->completado && searchMap(tareasMostradas, precedente->nombre) == NULL) 
            { // practicamente se repite pero con los precedentes
              precedente->prioridad *= -1;
              printf("%d. %s (Prioridad: %d) - Precedente: %s\n", contador + 1, precedente->nombre, precedente->prioridad, tarea->nombre);
              heap_push(tempHeap, precedente, precedente->prioridad);
              insertMap(tareasMostradas, precedente->nombre, precedente);
              contador++;
          }
        }

        // Insertar los precedentes en el montículo principal
        while (tempHeap->size > 0) {
          Nodo *precedente = (Nodo *)heap_top(tempHeap);
          heap_pop(tempHeap);
          heap_push(heap, precedente, precedente->prioridad);
        }
      }
      contador++;
    }
  }
}

void marcarComoCompletada(Map *grafo) {}

void deshacerUltima(Map *grafo) {}

void importarArchivo(Map *grafo) {}

int main() {

  int opcion;
  Stack *acciones;
  Map *grafoTareas = createMap(is_equal_string);

  printf("~~~~~~BIENVENIDO AL MENU DE JUGADOR~~~~~~\n\n");
  while (1) {
    menuTexto(&opcion);

    switch (opcion) {
    case 1:
      printf("\n--------------------------------------------\n");
      agregarTarea(grafoTareas);
      printf("\n--------------------------------------------\n");

      break;
    case 2:
      printf("\n--------------------------------------------\n");
      establecerPrecedencia(grafoTareas);
      printf("\n--------------------------------------------\n");

      break;
    case 3:
      printf("\n--------------------------------------------\n");
      mostrarTareas(grafoTareas);
      printf("\n--------------------------------------------\n");

      break;
    case 4:
      printf("\n--------------------------------------------\n");
      // marcarComoCompletada(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 5:
      printf("\n--------------------------------------------\n");
      // deshacerUltima(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 6:
      printf("\n--------------------------------------------\n");
      // importarArchivo(jugadores);
      printf("\n--------------------------------------------\n");
      break;
    case 0:

      exit(EXIT_SUCCESS);

    default:
      printf("Elija una opcion del menu para continuar:\n\n");
    }
  }

  return 0;
}