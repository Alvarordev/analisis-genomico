// =============================================================================
// MaxHeap.h  —  PARTE C: max-heap para obtener los k-mers más frecuentes
// =============================================================================
// Un max-heap es un árbol binario completo en el que cada padre es MAYOR o
// igual que sus hijos. Por eso el máximo siempre está en la raíz.
//
// Se guarda en un arreglo (vector), sin punteros, usando estas fórmulas para
// la posición i:
//     padre(i)         = (i - 1) / 2
//     hijoIzquierdo(i) = 2 * i + 1
//     hijoDerecho(i)   = 2 * i + 2
//
//   Ejemplo: el arreglo [9, 7, 8, 3, 5] representa este árbol:
//       9                  (posición 0, la raíz)
//       ├── 7              (posición 1 = hijo izquierdo de 0)
//       │   ├── 3          (posición 3 = hijo izquierdo de 1)
//       │   └── 5          (posición 4 = hijo derecho de 1)
//       └── 8              (posición 2 = hijo derecho de 0)
//
// Operaciones y su complejidad (n = elementos del heap):
//   - construir(lista)  O(n)       (algoritmo de Floyd, "heapify")
//   - insertar          O(log n)   (el elemento sube)
//   - extraerMaximo     O(log n)   (el último pasa a la raíz y baja)
//   - verMaximo         O(1)
//   - topN(n)           O(n log N) (n extracciones sobre una copia)
//
// Criterio de orden: más conteo = mayor prioridad. Si dos k-mers tienen el
// mismo conteo, va primero el menor alfabéticamente (para que el resultado
// sea siempre el mismo).
// =============================================================================
#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include <vector>

#include "TablaHashKmers.h"  // Para usar el struct KmerConteo

class MaxHeap {
public:
    // Crea un heap vacío.
    MaxHeap();

    // Crea un heap con todos los elementos de la lista en O(n).
    MaxHeap(std::vector<KmerConteo> elementos);

    void insertar(KmerConteo elemento);

    // Quita y devuelve el elemento con mayor conteo.
    // No se debe llamar si el heap está vacío (revisar con estaVacio()).
    KmerConteo extraerMaximo();

    // Devuelve el elemento con mayor conteo sin quitarlo.
    KmerConteo verMaximo();

    // Devuelve los n elementos más frecuentes, de mayor a menor, SIN modificar
    // este heap (trabaja sobre una copia).
    std::vector<KmerConteo> topN(int n);

    int getTamano();
    bool estaVacio();

private:
    std::vector<KmerConteo> datos;

    // ¿El elemento a debe ir más arriba que b en el heap?
    static bool tieneMasPrioridad(KmerConteo a, KmerConteo b);

    // Mueve hacia arriba el elemento de la posición i hasta su lugar.
    void subir(int i);

    // Mueve hacia abajo el elemento de la posición i hasta su lugar.
    void bajar(int i);
};

#endif
