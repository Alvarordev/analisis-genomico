// =============================================================================
// MaxHeap.cpp  —  PARTE C: implementación del max-heap
// =============================================================================
#include "MaxHeap.h"

#include <utility>  // swap: intercambia dos variables

using namespace std;

MaxHeap::MaxHeap() {
    // El vector "datos" empieza vacío; no hay nada más que hacer.
}

// -----------------------------------------------------------------------------
// Construcción en O(n) (algoritmo de Floyd):
//   Las hojas (la segunda mitad del arreglo) ya son heaps de un elemento.
//   Recorremos los nodos internos desde el último hasta la raíz y bajamos cada
//   uno. Es más rápido que insertar los n elementos uno por uno (O(n log n)).
// -----------------------------------------------------------------------------
MaxHeap::MaxHeap(vector<KmerConteo> elementos) {
    datos = elementos;
    int ultimoPadre = getTamano() / 2 - 1;
    for (int i = ultimoPadre; i >= 0; i--) {
        bajar(i);
    }
}

bool MaxHeap::tieneMasPrioridad(KmerConteo a, KmerConteo b) {
    if (a.conteo != b.conteo) {
        return a.conteo > b.conteo;
    }
    // Empate: el menor alfabéticamente tiene más prioridad.
    // En C++ los strings se comparan con < y > (en Java sería compareTo).
    return a.kmer < b.kmer;
}

void MaxHeap::insertar(KmerConteo elemento) {
    // Se agrega al final del arreglo (última hoja) y se sube.
    datos.push_back(elemento);
    subir(getTamano() - 1);
}

KmerConteo MaxHeap::extraerMaximo() {
    KmerConteo maximo = datos[0];

    // El último elemento pasa a la raíz, se borra del final y se baja.
    datos[0] = datos[getTamano() - 1];
    datos.pop_back();  // Borra el último elemento del vector
    if (!estaVacio()) {
        bajar(0);
    }
    return maximo;
}

KmerConteo MaxHeap::verMaximo() {
    return datos[0];
}

vector<KmerConteo> MaxHeap::topN(int n) {
    // "MaxHeap copia = *this;" crea una COPIA de este heap (this es un puntero
    // al objeto actual, como en Java; *this es el objeto en sí). Así podemos
    // extraer sin dañar el heap original.
    MaxHeap copia = *this;

    vector<KmerConteo> resultado;
    for (int i = 0; i < n && !copia.estaVacio(); i++) {
        resultado.push_back(copia.extraerMaximo());
    }
    return resultado;
}

int MaxHeap::getTamano() { return datos.size(); }
bool MaxHeap::estaVacio() { return datos.empty(); }

// -----------------------------------------------------------------------------
// Subir (sift-up): mientras el elemento tenga más prioridad que su padre,
// se intercambian. Como máximo recorre la altura del árbol: O(log n).
// -----------------------------------------------------------------------------
void MaxHeap::subir(int i) {
    while (i > 0) {
        int padre = (i - 1) / 2;
        if (!tieneMasPrioridad(datos[i], datos[padre])) {
            break;  // Ya está en su lugar
        }
        swap(datos[i], datos[padre]);
        i = padre;
    }
}

// -----------------------------------------------------------------------------
// Bajar (sift-down): mientras algún hijo tenga más prioridad que el elemento,
// se intercambia con el MAYOR de sus hijos. También es O(log n).
// -----------------------------------------------------------------------------
void MaxHeap::bajar(int i) {
    int n = getTamano();
    while (true) {
        int izquierdo = 2 * i + 1;
        int derecho = 2 * i + 2;
        int mayor = i;

        if (izquierdo < n && tieneMasPrioridad(datos[izquierdo], datos[mayor])) {
            mayor = izquierdo;
        }
        if (derecho < n && tieneMasPrioridad(datos[derecho], datos[mayor])) {
            mayor = derecho;
        }
        if (mayor == i) {
            break;  // Ningún hijo es mayor: ya está en su lugar
        }
        swap(datos[i], datos[mayor]);
        i = mayor;
    }
}
