// =============================================================================
// TablaHashSecuencias.h  —  PARTE B: tabla hash 1 (linear probing)
// =============================================================================
// Guarda pares  clave = ID de la secuencia  ->  valor = la secuencia.
//
// Linear probing (sondeo lineal):
//   1. Se calcula la posición inicial: pos = hash(id) % tamaño.
//   2. Si esa casilla está ocupada, se prueba la siguiente (pos + 1), luego
//      pos + 2, y así hasta encontrar una libre. Al llegar al final de la
//      tabla se vuelve al inicio (por eso se usa % tamaño).
//
// Redimensionamiento (lo que pide el enunciado):
//   - La tabla empieza con 10 casillas.
//   - Factor de carga = cantidad de elementos / tamaño de la tabla.
//   - Si al insertar el factor de carga pasaría de 1, la tabla crece al
//     siguiente número primo >= 2 * cantidad. Así el factor queda en ~0.5.
//     Ejemplo: con 11 elementos el nuevo tamaño es 23 (11/23 = 0.48).
// =============================================================================
#ifndef TABLA_HASH_SECUENCIAS_H
#define TABLA_HASH_SECUENCIAS_H

#include <string>
#include <vector>

#include "SecuenciaADN.h"

// Una casilla (bin) de la tabla.
struct CasillaSecuencia {
    SecuenciaADN dato;
    bool ocupada = false;  // Al crearse, la casilla está vacía
};

class TablaHashSecuencias {
public:
    // Constructor: crea la tabla con 10 casillas vacías.
    TablaHashSecuencias();

    // Inserta una secuencia. Si el ID ya existía, reemplaza su secuencia.
    void insertar(SecuenciaADN nueva);

    // Busca una secuencia por su ID.
    // Devuelve un PUNTERO (SecuenciaADN*) a la secuencia encontrada, o
    // nullptr si no existe. nullptr es el "null" de Java.
    // Un puntero es, más o menos, lo que en Java es una referencia a un objeto.
    SecuenciaADN* buscar(std::string id);

    // Muestra toda la tabla en pantalla.
    void imprimir();

    // Getters
    int getCantidad();
    int getTamano();
    double getFactorCarga();
    int getColisiones();
    int getRedimensionamientos();

private:
    // La tabla es una lista de casillas. vector es como ArrayList.
    std::vector<CasillaSecuencia> tabla;

    int cantidad;             // Cuántas secuencias hay guardadas
    int colisiones;           // Cuántas casillas ocupadas se saltaron al insertar
    int redimensionamientos;  // Cuántas veces creció la tabla

    // Calcula la posición inicial de una clave.
    int funcionHash(std::string clave);

    // Inserta sin revisar el factor de carga (lo usan insertar y redimensionar).
    void insertarSinRedimensionar(SecuenciaADN nueva);

    // Crea una tabla nueva del tamaño indicado y reinserta todo (rehash).
    void redimensionar(int nuevoTamano);

    // Funciones auxiliares para buscar números primos.
    static bool esPrimo(int n);
    static int siguientePrimo(int n);
};

#endif
