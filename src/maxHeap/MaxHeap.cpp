#include "MaxHeap.h"
#include <utility>
#include <vector>

using namespace std;

MaxHeap::MaxHeap() {}

MaxHeap::MaxHeap(vector<KmerConteo> elementos) {
    datos = elementos;
    int ultimoPadre = getLength() / 2 - 1;
    for (int i = ultimoPadre; i >= 0; i--) {
        bajar(i);
    }
}

void MaxHeap::insertar(KmerConteo elemento) {
    datos.push_back(elemento);
    subir(getLength() - 1);
}

KmerConteo MaxHeap::extraerMaximo() {
    KmerConteo maximo = datos[0];

    datos[0] = datos[getLength() - 1];
    datos.pop_back();

    if (!estaVacio()) {
        bajar(0);
    }

    return maximo;
}

KmerConteo MaxHeap::verMaximo() {
    return datos[0];
}

vector<KmerConteo> MaxHeap::topN(int n) {
    MaxHeap copia = *this;
    vector<KmerConteo> resultado;

    while (n > 0 && !copia.estaVacio()) {
        resultado.push_back(copia.extraerMaximo());
        n--;
    }

    return resultado;
}

void MaxHeap::subir(int i) {
    while (i > 0) {
        int p = padre(i);
        if (!tieneMasPrioridad(datos[i], datos[p])) {
            break;
        }
        swap(datos[i], datos[p]);
        i = p;
    }
}

void MaxHeap::bajar(int i) {
    int n = getLength();
    while (true) {
        int izquierdo = hijoIzquierdo(i);
        int derecho = hijoDerecho(i);
        int mayor = i;

        if (izquierdo < n && tieneMasPrioridad(datos[izquierdo], datos[mayor])) {
            mayor = izquierdo;
        }
        if (derecho < n && tieneMasPrioridad(datos[derecho], datos[mayor])) {
            mayor = derecho;
        }
        if (mayor == i) {
            break;
        }
        swap(datos[i], datos[mayor]);
        i = mayor;
    }
}

int MaxHeap::padre(int i) {
    return (i - 1) / 2;
}

int MaxHeap::hijoIzquierdo(int i) {
    return 2 * i + 1;
}

int MaxHeap::hijoDerecho(int i) {
    return 2 * i + 2;
}

bool MaxHeap::tieneMasPrioridad(KmerConteo a, KmerConteo b) {
    if (a.conteo != b.conteo) {
        return a.conteo > b.conteo;
    } else {
        return a.kmer < b.kmer;
    }
}

int MaxHeap::getLength() {
    return datos.size();
}

bool MaxHeap::estaVacio() {
    if (getLength() == 0) {
        return true;
    }

    return false;
}
