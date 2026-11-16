// =============================================================================
// SecuenciaADN.h
// =============================================================================
// ¿Qué es un archivo .h (header)?
//   En Java, una clase se escribe en un solo archivo .java. En C++ se suele
//   separar en dos:
//     - El .h  (header): DECLARA qué existe (atributos y firmas de métodos).
//     - El .cpp       : IMPLEMENTA los métodos (el código de verdad).
//   Otros archivos hacen #include "Algo.h" para poder usar esa clase, de forma
//   parecida a un "import" en Java.
//
// ¿Qué son #ifndef / #define / #endif?
//   Son "guardas de inclusión". Evitan que el mismo .h se copie dos veces si
//   varios archivos lo incluyen. Todos los .h del proyecto las llevan.
// =============================================================================
#ifndef SECUENCIA_ADN_H
#define SECUENCIA_ADN_H

#include <string>  // Para usar std::string (equivale a String de Java)

// Un "struct" es como una clase de Java que solo guarda datos y cuyos
// atributos son públicos. Corresponde al struct DNASequence del enunciado.
struct SecuenciaADN {
    std::string id;         // Identificador, por ejemplo: MAC.US.x.239.M33262
    std::string secuencia;  // Las bases, por ejemplo: ATGGGATGTCTTGGGAA...
};

// En C++ las clases y structs terminan con punto y coma (en Java no).

#endif
