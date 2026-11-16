// =============================================================================
// TablaHashKmers.cpp  —  PARTE B: implementación de la tabla hash 2
// =============================================================================
#include "TablaHashKmers.h"

#include <iomanip>
#include <iostream>

using namespace std;

TablaHashKmers::TablaHashKmers()
    : k(0), kmersDistintos(0), kmersTotales(0), colisiones(0) {
    // La tabla queda vacía (tamaño 0). Sirve para saber que aún no se construyó.
}

TablaHashKmers::TablaHashKmers(int k)
    : k(k), kmersDistintos(0), kmersTotales(0), colisiones(0) {
    // Calculamos 4^k multiplicando 4 por sí mismo k veces.
    int tamano = 1;
    for (int i = 0; i < k; i++) {
        tamano = tamano * 4;
    }
    // resize cambia el tamaño del vector; las casillas nuevas quedan vacías.
    tabla.resize(tamano);
}

// -----------------------------------------------------------------------------
// Funciones hash
// -----------------------------------------------------------------------------

// h1: hash polinomial en base 31 (igual que en la tabla 1).
int TablaHashKmers::hash1(string kmer) {
    unsigned long long hash = 0;
    for (char c : kmer) {
        hash = hash * 31 + c;
    }
    return hash % getTamano();
}

// h2: otro hash distinto (base 37), convertido en un número IMPAR entre 1 y
// tamaño-1. La fórmula 2 * x + 1 siempre da impar.
int TablaHashKmers::hash2(string kmer) {
    unsigned long long hash = 0;
    for (char c : kmer) {
        hash = hash * 37 + c;
    }
    int mitad = getTamano() / 2;
    return 2 * (hash % mitad) + 1;
}

// -----------------------------------------------------------------------------
// Inserción y búsqueda
// -----------------------------------------------------------------------------

void TablaHashKmers::insertar(string kmer) {
    int pos = hash1(kmer);
    int salto = hash2(kmer);

    // Probamos como máximo todas las casillas de la tabla.
    for (int i = 0; i < getTamano(); i++) {
        if (tabla[pos].conteo == 0) {
            // Casilla vacía: el k-mer es nuevo.
            tabla[pos].kmer = kmer;
            tabla[pos].conteo = 1;
            kmersDistintos++;
            kmersTotales++;
            return;
        }
        if (tabla[pos].kmer == kmer) {
            // El k-mer ya estaba: aumentamos su conteo.
            tabla[pos].conteo++;
            kmersTotales++;
            return;
        }
        // La casilla tiene otro k-mer (colisión): saltamos.
        colisiones++;
        pos = (pos + salto) % getTamano();
    }
    // Nunca deberíamos llegar aquí porque la tabla tiene espacio para todos
    // los k-mers posibles.
    cout << "Error: la tabla de k-mers esta llena" << endl;
}

int TablaHashKmers::buscar(string kmer) {
    int pos = hash1(kmer);
    int salto = hash2(kmer);

    // Seguimos los mismos saltos que al insertar.
    for (int i = 0; i < getTamano(); i++) {
        if (tabla[pos].conteo == 0) {
            return 0;  // Casilla vacía: el k-mer no está
        }
        if (tabla[pos].kmer == kmer) {
            return tabla[pos].conteo;
        }
        pos = (pos + salto) % getTamano();
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Construir la tabla a partir de una secuencia (ventana deslizante)
// -----------------------------------------------------------------------------
void TablaHashKmers::construirDesde(string secuencia) {
    int largo = secuencia.size();

    // La última ventana empieza en la posición largo - k.
    for (int inicio = 0; inicio + k <= largo; inicio++) {
        // substr(inicio, k) = substring(inicio, inicio + k) de Java.
        // ¡Ojo! En C++ el segundo parámetro es la LONGITUD, no la posición final.
        string kmer = secuencia.substr(inicio, k);

        // Revisamos que el k-mer solo tenga A, C, G o T.
        bool esValido = true;
        for (char c : kmer) {
            if (c != 'A' && c != 'C' && c != 'G' && c != 'T') {
                esValido = false;
                break;
            }
        }

        if (esValido) {
            insertar(kmer);
        }
    }
}

vector<KmerConteo> TablaHashKmers::obtenerKmers() {
    vector<KmerConteo> resultado;
    for (KmerConteo casilla : tabla) {
        if (casilla.conteo > 0) {
            resultado.push_back(casilla);
        }
    }
    return resultado;
}

void TablaHashKmers::imprimir() {
    double factorCarga = (double) kmersDistintos / getTamano();

    cout << "Tabla hash 2 (double hashing)  |  k = " << k << endl;
    cout << "  Tamano (4^k): " << getTamano()
         << "  |  k-mers distintos: " << kmersDistintos
         << "  |  k-mers totales: " << kmersTotales
         << "  |  Factor de carga: " << fixed << setprecision(3) << factorCarga
         << "  |  Colisiones: " << colisiones << endl;

    cout << left << setw(8) << "Bin" << setw(14) << "Key (k-mer)" << "Valor" << endl;

    // Solo mostramos las casillas ocupadas (con 4096 casillas serían muchas).
    for (int i = 0; i < getTamano(); i++) {
        if (tabla[i].conteo > 0) {
            cout << left << setw(8) << i << setw(14) << tabla[i].kmer
                 << tabla[i].conteo << endl;
        }
    }
}

int TablaHashKmers::getK() { return k; }
int TablaHashKmers::getTamano() { return tabla.size(); }
int TablaHashKmers::getKmersDistintos() { return kmersDistintos; }
int TablaHashKmers::getKmersTotales() { return kmersTotales; }
int TablaHashKmers::getColisiones() { return colisiones; }
