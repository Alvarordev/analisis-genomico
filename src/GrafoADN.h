// =============================================================================
// GrafoADN.h  —  PARTE D: grafo de 2-mers (Khan et al., 2024)
// =============================================================================
// Pasos (según el algoritmo del enunciado):
//   1. Se parte la secuencia en pares de nucleótidos SIN solaparse:
//        TCATTAGCAGTTAGCAGC  ->  TC AT TA GC AG TT AG CA GC
//        posiciones:              1  2  3  4  5  6  7  8  9
//   2. Cada par distinto es un VÉRTICE del grafo.
//   3. Para cada par de posiciones i < j se agrega un arco DIRIGIDO desde el
//      par de la posición i hacia el par de la posición j, con peso:
//            w = 1 / (j - i)^beta
//      Ejemplo con beta = 1:  w(TC, AT) = 1/(2-1) = 1
//                             w(AT, CA) = 1/(8-2) = 0.16667
//   4. Si el mismo arco (u, v) aparece varias veces (porque u o v se repiten
//      en la secuencia), los pesos se SUMAN en un solo arco.
//   5. Pueden existir lazos (u, u): por ejemplo AG en las posiciones 5 y 7.
//
// Representación: matriz de adyacencia n x n (n = número de vértices).
//   matriz[a][b] = peso del arco del vértice a al vértice b (0 si no hay arco).
//   Como hay a lo más 16 pares posibles (4 x 4), la matriz es como máximo
//   de 16 x 16.
//
// Complejidad (P = número de pares = largo / 2, V = vértices <= 16):
//   - construir: O(P^2) para recorrer todos los i < j (la búsqueda del
//     índice de cada vértice cuesta O(V) = O(1), porque V <= 16).
//   - memoria:   O(V^2) para la matriz.
// =============================================================================
#ifndef GRAFO_ADN_H
#define GRAFO_ADN_H

#include <string>
#include <vector>

class GrafoADN {
public:
    // Grafo vacío (sin construir).
    GrafoADN();

    // Construye el grafo de la secuencia con el valor de beta indicado.
    GrafoADN(std::string secuencia, double beta);

    // Imprime la tabla de pares con su posición (como en el enunciado).
    void imprimirPares();

    // Imprime la matriz de adyacencia con los nombres de los vértices.
    void imprimirMatriz();

    // Devuelve el peso del arco de u a v (0 si no existe). Ej: getPeso("TC", "AT")
    double getPeso(std::string u, std::string v);

    int getCantidadVertices();
    int getCantidadPares();
    double getBeta();

private:
    double beta;
    std::vector<std::string> pares;     // Los pares en orden: TC, AT, TA, ...
    std::vector<std::string> vertices;  // Pares distintos, en orden de aparición
    std::vector<std::vector<double>> matriz;  // Matriz de adyacencia (vector de vectores)

    // Devuelve el índice del vértice en la lista "vertices", o -1 si no está.
    int indiceDe(std::string par);

    // Agrega el par como vértice si todavía no existe y devuelve su índice.
    int agregarVertice(std::string par);
};

#endif
