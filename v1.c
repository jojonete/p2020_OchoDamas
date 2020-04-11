#include <stdio.h>

/*
Un tablero tiene 64 casillas
Utilizamos una variable entera de 64 bits para representar el tablero
*/
typedef unsigned long long int u64;

/*
Esta variable marca bloques de casillas alcanzables con una dama en un movimiento
Son las filas del tablero, las columnas, y las diagonales
42 son:
- 8 filas
- 8 columnas
- 13 diagonales principales
- 13 diagonales secundarias
*/
u64 filas[42];

/*
Contador de llamadas a colocar ()
*/
int llamadas_a_colocar;

/*
Contador de soluciones
*/
int soluciones;

/*
Tableros iniciales
Estos tableros marcan casillas en las que no vamos a permitir que figure una dama.
La idea es que, si hubiese una solucion que contiene una dama en una casilla prohibida,
la solucion se puede rotar y/o reflejar hasta que todas las damas queden en casillas permitidas,
y por lo tanto no perdemos soluciones por eliminar estas casillas.
Dicho de otra forma, las soluciones que perdemos se pueden obtener siempre
como rotaciones y/o reflexiones de las que encontramos.

En principio, intentamos eliminar el mayor numero de casillas posible,
pues esto hara que el programa tenga que verificar menos combinaciones y termine antes.
Para cada tablero inicial posible, contamos el numero de veces que se llama
a la funcion colocar() como medida del tiempo que se tarda en encontrar todas las soluciones.

0: Solucion por fuerza bruta.
El valor tableros_iniciales[0] permite todas las casillas.
Este valor encontrara todas las soluciones, incluidas rotaciones y reflexiones.
Resultados para solucion 0:
Total = 118969 llamadas.
Total = 92 soluciones.

1: Solucion con optimizacion sencilla.
El valor tableros_iniciales[1] elimina tres de las cuatro esquinas.
Una dama en una esquina ataca las otras tres esquinas,
por lo tanto ninguna solucion puede ocupar mas de una esquina.
Cualquier solucion que ocupe una esquina se puede rotar hasta que quede en la esquina deseada,
asi que podemos eliminar las demas y solo estaremos eliminando algunas rotaciones.
Resultados para solucion 1:
Total = 94258 llamadas.
Total = 80 soluciones.

2: Solucion con optimizacion agresiva.
El valor tableros_iniciales[2] elimina ocho casillas elegidas estrategicamente.
Dada cualquier solucion (con 8 damas colocadas),
nos fijamos en las damas que estan pegadas a los laterales del tablero.
De estas damas, elegimos la que este mas cercana a una esquina.
Si hay dos "empatadas" (a la misma distancia de la esquina), elegimos cualquiera de ellas.
En estas condiciones, siempre podemos rotar y/o reflejar la solucion
hasta que la dama elegida quede en la mitad izquierda de la fila superior.
Si hacemos esto, la solucion obtenida no tendra ninguna dama
en ninguna de las casillas eliminadas.
Por lo tanto, al eliminar estas casillas solo estamos eliminando
rotaciones y/o reflexiones de algunas de las soluciones que encontramos.
Resultados para solucion 2:
Total = 58956 llamadas.
Total = 35 soluciones.
*/
u64 tableros_iniciales[3] = {
  0xFFFFFFFFFFFFFFFFULL,
  0xFEFFFFFFFFFFFF7EULL,
  0xF07FFFFFFFFFFE7EULL,
  };

void init_filas (void)
{
  int k;

  for (k = 0; k < 8; k++)
  {
    /* Filas */
    filas[k] = 0x00000000000000FFULL << (k * 8);

    /* Columnas */
    filas[8 + k] = 0x0101010101010101ULL << k;
  }

  /* Diagonal princpal de longitud maxima */
  filas[16] = 0x8040201008040201ULL;

  /* Diagonal secundaria de longitud maxima */
  filas[17] = 0x0102040810204080ULL;

  /* Diagonales mas cortas */
  for (k = 0; k < 6; k++)
  {
    const int shift = 8 * (k + 1);
    /* Diagonal principal por arriba */
    filas[18 + k] = filas[16] << shift;

    /* Diagonal principal por abajo */
    filas[24 + k] = filas[16] >> shift;

    /* Diagonal secundaria por arriba */
    filas[30 + k] = filas[17] << shift;

    /* Diagonal secundaria por abajo */
    filas[36 + k] = filas[17] >> shift;
  }

}

void imprimir_tablero (u64 t)
{
  int y;

  for (y = 0; y < 8; y++)
  {
    int x;

    for (x = 0; x < 8; x++)
      printf ("%c", (t >> (63 - (8 * y + x))) & 1ULL ? 'X' : '.');

    printf ("\n");
  }
}

void colocar (int faltan, int siguiente, u64 colocadas, u64 posibles)
{
  llamadas_a_colocar ++;

  if (faltan <= 0)
  {
    soluciones ++;
    printf ("Solucion %d:\n", soluciones);
    imprimir_tablero (colocadas);
    return;
  }

  while (siguiente < 64)
  {
    if ((posibles >> siguiente) & 1ULL)
    {
      const u64 this_bit = 1ULL << siguiente;
      u64 nuevo_posibles = posibles;
      int k;

      for (k = 0; k < 42; k++)
        if (filas[k] & this_bit)
	        nuevo_posibles = nuevo_posibles & ~ filas[k];

      colocar (faltan - 1, siguiente + 1, colocadas | this_bit, nuevo_posibles);
    }

    siguiente++;
  }
}

int main (void)
{
  int k;

  init_filas ();

  /* Comprobacion de filas */
  printf ("=====================\n");
  printf ("COMPROBACION DE FILAS\n");
  printf ("=====================\n");

  for (k = 0; k < 42; k++)
  {
    printf ("Fila %d:\n", k);
    imprimir_tablero (filas[k]);
  }

  printf ("\n");

  for (k = 0; k < 3; k++)
  {
    printf ("==========\n");
    printf ("SOLUCION %d\n", k);
    printf ("==========\n");
    printf ("Tablero inicial:\n");
    imprimir_tablero (tableros_iniciales[k]);

    llamadas_a_colocar = 0;
    soluciones = 0;

    colocar (8, 0, 0ULL, tableros_iniciales[k]);

    printf ("Resultados para solucion %d:\n", k);
    printf ("Total = %d llamadas.\n", llamadas_a_colocar);
    printf ("Total = %d soluciones.\n", soluciones);
    printf ("\n");
  }

  return 0;
}
