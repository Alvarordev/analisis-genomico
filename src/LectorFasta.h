// =============================================================================
// LectorFasta.h  —  PARTE A: parser de secuencias
// =============================================================================
// Lee un archivo en formato FASTA, que tiene esta forma:
//
//   >MAC.US.x.239.M33262
//   ATGGGATGTCTTGGGAATCAGCTGCTTATCGCCATCTTGCTTTTAAGTGTCTATGGGATC
//   TATTGTACTCTATATGTCACAGTCTTTTATGGTGTACCAGCTTGGAGGAATGCGACA
//   >OTRO.ID
//   ...
//
// - Una línea que empieza con '>' trae el ID de una secuencia nueva.
// - Las líneas siguientes (pueden ser varias) son la secuencia.
// =============================================================================
#ifndef LECTOR_FASTA_H
#define LECTOR_FASTA_H

#include <string>
#include <vector>  // std::vector equivale a ArrayList de Java

#include "SecuenciaADN.h"

class LectorFasta {
public:  // En C++ se escribe "public:" una vez y aplica a todo lo de abajo.

    // Método estático: se llama sin crear un objeto, igual que en Java:
    //     LectorFasta::leerArchivo("datos.fasta");
    // (en C++ se usa :: en vez del punto para lo estático).
    //
    // Devuelve la lista de secuencias leídas. Si el archivo no se puede
    // abrir, muestra un error y devuelve una lista vacía.
    static std::vector<SecuenciaADN> leerArchivo(std::string ruta);

private:
    // Quita espacios y gaps ('-' y '.') de una línea y la pasa a mayúsculas.
    static std::string limpiarLinea(std::string linea);
};

#endif
