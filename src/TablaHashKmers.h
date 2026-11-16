// =============================================================================
// TablaHashKmers.h  —  PARTE B: tabla hash 2 (double hashing)
// =============================================================================
// Guarda pares  clave = k-mer  ->  valor = cuántas veces aparece.
//
// ¿Cómo se obtienen los k-mers? Con una "ventana" de tamaño k que avanza de a
// un carácter. Con k = 3 y la secuencia ATGGGATG:
//   ATGGGATG  ->  ATG
//    TGGGATG  ->  TGG
//     GGGATG  ->  GGG   ... y así hasta el final.
//
// Double hashing (doble hashing):
//   Se usan DOS funciones hash:
//     - h1 da la posición inicial.
//     - h2 da el tamaño del "salto" cuando hay colisión.
//   El intento número i revisa la posición:  (h1 + i * h2) % tamaño
//   A diferencia del linear probing (que salta siempre de 1 en 1), cada clave
//   salta una distancia distinta. Así se forman menos "grupos" de casillas
//   ocupadas seguidas.
//
// Tamaño de la tabla = 4^k (4 letras posibles: A, C, G, T).
//   k = 3  ->  64 casillas,   k = 6  ->  4096 casillas.
//   Hay exactamente 4^k k-mers posibles, así que la tabla nunca se llena.
//
// Detalle importante: 4^k es potencia de 2. Para que los saltos recorran
// TODAS las casillas, h2 debe ser IMPAR (así no tiene divisores comunes con
// el tamaño). Por eso h2 siempre devuelve un número impar.
// =============================================================================
#ifndef TABLA_HASH_KMERS_H
#define TABLA_HASH_KMERS_H

#include <string>
#include <vector>

// Un par (k-mer, conteo). También es una casilla de la tabla:
// si conteo == 0, la casilla está vacía.
// Más adelante el max-heap (parte c) usará este mismo struct.
struct KmerConteo {
    std::string kmer = "";
    int conteo = 0;
};

class TablaHashKmers {
public:
    // Constructor sin parámetros: tabla vacía y sin construir (k = 0).
    TablaHashKmers();

    // Constructor con k: crea una tabla de 4^k casillas vacías.
    TablaHashKmers(int k);

    // Saca todos los k-mers de la secuencia y los inserta en la tabla.
    // Las ventanas con letras que no son A, C, G o T (por ejemplo N) se saltan.
    void construirDesde(std::string secuencia);

    // Suma 1 al conteo del k-mer (si no estaba, lo agrega con conteo 1).
    void insertar(std::string kmer);

    // Devuelve cuántas veces aparece el k-mer (0 si no aparece).
    int buscar(std::string kmer);

    // Devuelve todos los pares (k-mer, conteo) guardados. Lo usará el max-heap.
    std::vector<KmerConteo> obtenerKmers();

    // Muestra en pantalla las casillas ocupadas.
    void imprimir();

    // Getters
    int getK();
    int getTamano();
    int getKmersDistintos();
    int getKmersTotales();
    int getColisiones();

private:
    int k;
    std::vector<KmerConteo> tabla;
    int kmersDistintos;  // Cuántos k-mers diferentes hay
    int kmersTotales;    // Cuántos k-mers se insertaron en total (con repetidos)
    int colisiones;      // Cuántas casillas ocupadas se saltaron al insertar

    int hash1(std::string kmer);  // Posición inicial
    int hash2(std::string kmer);  // Tamaño del salto (siempre impar)
};

#endif
