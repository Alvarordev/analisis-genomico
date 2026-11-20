#include "TablaKmers.h"
#include <iomanip>
#include <iostream>
#include <vector>
using namespace std;

const int A = 37;
const int B = 31;

TablaKmers::TablaKmers(int k): k(k), bins(0), distintos(0), totales(0), colisiones(0) {
    int tamano = 1;
    for (int i = 0; i < k; i++){
        tamano = tamano * 4;
    }

    bins = vector<KmerConteo>(tamano);
}

int TablaKmers::hash1(string kmer, int length) {
    unsigned long long h = 0;

    for (char c : kmer) {
        h = (h * A + c) % length;
    }

    return h;
}

int TablaKmers::hash2(string kmer, int length) {
    unsigned long long h = 0;
    int mitad = length / 2;

    for (char c : kmer) {
        h = (h * B + c) % mitad;
    }

    return 2 * h + 1;
}

bool TablaKmers::insertar(string kmer) {
    int index = hash1(kmer, getLength());
    int salto = hash2(kmer, getLength());

    for (int i = 0; i < getLength(); i++){
        if (bins[index].conteo == 0) {
            bins[index].kmer = kmer;
            bins[index].conteo = 1;
            distintos++;
            totales++;
            return true;
        }
        if (bins[index].kmer == kmer) {
            bins[index].conteo++;
            totales++;
            return true;
        }

        colisiones++;
        index = (index + salto) % getLength();
    }

    return false;
}

int TablaKmers::buscar(string kmer) {
    int index = hash1(kmer, getLength());
    int salto = hash2(kmer, getLength());

    for (int i = 0; i < getLength(); i++) {
        if (bins[index].conteo == 0) {
            return 0;
        }
        if (bins[index].kmer == kmer) {
            return bins[index].conteo;
        }
        index = (index + salto) % getLength();
    }

    return 0;
}

void TablaKmers::construirDesde(string secuencia) {
    int length = secuencia.size();

    for (int i = 0; i + k <= length; i++) {
        string kmer = secuencia.substr(i, k);

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

vector<KmerConteo> TablaKmers::obtenerKmers() {
    vector<KmerConteo> resultado;

    for (KmerConteo elemento : bins) {
        if (elemento.conteo > 0) {
            resultado.push_back(elemento);
        }
    }

    return resultado;
}

void TablaKmers::imprimir() {
    cout << "Tabla hash 2 (double hashing)  |  k = " << k << endl;
    cout << "  Tamano (4^k): " << getLength()
         << "  |  k-mers distintos: " << distintos
         << "  |  k-mers totales: " << totales
         << "  |  Factor de carga: " << fixed << setprecision(3) << getFactorCarga()
         << "  |  Colisiones: " << colisiones << endl;

    cout << left << setw(8) << "Bin" << setw(14) << "Key (k-mer)" << "Valor" << endl;

    // Solo mostramos las casillas ocupadas (con 4096 casillas serían muchas).
    for (int i = 0; i < getLength(); i++) {
        if (bins[i].conteo > 0) {
            cout << left << setw(8) << i << setw(14) << bins[i].kmer
                 << bins[i].conteo << endl;
        }
    }
}

int TablaKmers::getK() {
    return k;
}

int TablaKmers::getColisiones() {
    return colisiones;
}

int TablaKmers::getDistintos() {
    return distintos;
}

int TablaKmers::getLength() {
    return bins.size();
}

int TablaKmers::getTotales() {
    return totales;
}

double TablaKmers::getFactorCarga() {
    return (double) distintos/getLength();
}

const vector<KmerConteo>& TablaKmers::getBins() {
    return bins;
}
