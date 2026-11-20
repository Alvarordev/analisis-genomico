#include "TablaHash.h"

#include <iomanip>
#include <iostream>

using namespace std;

const int A = 37;

TablaHash::TablaHash(int tamano): bins(tamano), num_keys(0), colisiones(0), redimensionamientos(0) {}

int TablaHash::funcionHash(string key, int length) {
    unsigned long long h = 0;

    for (char c : key) {
        h = (h * A + c) % length;
    }

    return h;
}

bool TablaHash::insertar(string key, string valor) {
    string* existente = buscar(key);

    if (existente != nullptr) {
        *existente = valor;
        return true;
    }

    if (num_keys + 1 > getLength()) {
        int nuevoTamano = siguientePrimo(2 * (num_keys + 1));
        redimensionar(nuevoTamano);
    }

    return insertarSinRedimensionar(key, valor);
}

bool TablaHash::insertarSinRedimensionar(string key, string valor) {
    int index = funcionHash(key, getLength());
    int conteo = 0;
    Bin actual = bins[index];

    while(actual.estado != VACIO && actual.key != key && conteo != getLength()) {
        index = (index + 1) % getLength();
        actual = bins[index];
        conteo++;
        colisiones++;
    }

    if (conteo == getLength()) {
        return false;
    }

    if(actual.estado == VACIO ||bins[index].estado == BORRADO) {
        bins[index].key = key;
        bins[index].valor = valor;
        bins[index].estado = OCUPADO;
        num_keys++;
    } else {
        bins[index].valor = valor;
    }

    return true;
}

string* TablaHash::buscar(string key) {
    int index = funcionHash(key, getLength());
    int conteo = 0;

    while (bins[index].estado != VACIO && conteo != getLength()) {
        if (bins[index].key == key && bins[index].estado != BORRADO) {
            return &bins[index].valor;
        } else {
            index = (index + 1) % getLength();
            conteo++;
        }
    }

    return nullptr;
}

void TablaHash::redimensionar(int nuevoTamano) {
    vector<Bin> arregloViejo = bins;

    bins = vector<Bin>(nuevoTamano);
    num_keys = 0;

    for (Bin elemento: arregloViejo) {
        if(elemento.estado == OCUPADO) {
            insertarSinRedimensionar(elemento.key, elemento.valor);
        }
    }
    redimensionamientos++;
}

// -----------------------------------------------------------------------------
// imprimir: muestra las estadísticas y TODOS los bins de la tabla.
//
// Ejemplo de salida (tabla de 10 bins):
//
//   Tabla hash (linear probing)
//     Bins: 10  |  num_keys: 6  |  Factor de carga: 0.600  |  Colisiones: 6  |  Redimensionamientos: 0
//   Bin   Estado    Key (ID secuencia)            Valor
//   0     OCUPADO   temp10                        valorT
//   1     VACIO     -
//   ...
//
// Herramientas de <iomanip> que se usan:
//   - setw(n): la PRÓXIMA cosa que se imprima ocupa n caracteres (rellena
//     con espacios). Sirve para que las columnas queden alineadas.
//   - left: el relleno va a la derecha (el texto queda pegado a la izquierda).
//   - fixed + setprecision(3): los decimales se muestran con 3 cifras (0.600).
// -----------------------------------------------------------------------------
void TablaHash::imprimir() {
    const int ANCHO_VALOR = 40;  // Cuántas letras del valor se muestran como máximo

    // 1. Estadísticas generales de la tabla.
    cout << "Tabla hash (linear probing)" << endl;
    cout << "  Bins: " << getLength()
         << "  |  num_keys: " << num_keys
         << "  |  Factor de carga: " << fixed << setprecision(3) << getFactorCarga()
         << "  |  Colisiones: " << colisiones
         << "  |  Redimensionamientos: " << redimensionamientos << endl;

    // 2. Encabezado de las columnas.
    cout << left << setw(6) << "Bin" << setw(10) << "Estado"
         << setw(30) << "Key (ID secuencia)" << "Valor" << endl;

    // 3. Una línea por cada bin, de 0 a length - 1.
    for (int i = 0; i < getLength(); i++) {
        cout << left << setw(6) << i;

        if (bins[i].estado == VACIO) {
            cout << setw(10) << "VACIO" << "-" << endl;
            continue;  // Pasa directo al siguiente bin
        }

        if (bins[i].estado == BORRADO) {
            // El bin conserva la key vieja, pero ya no cuenta como guardada.
            cout << setw(10) << "BORRADO" << "<borrado> (era " << bins[i].key << ")" << endl;
            continue;
        }

        // OCUPADO: key y valor. Si el valor es muy largo (una secuencia de
        // ADN real tiene miles de letras), solo se muestra el comienzo.
        cout << setw(10) << "OCUPADO" << setw(30) << bins[i].key;
        if ((int) bins[i].valor.size() > ANCHO_VALOR) {
            cout << bins[i].valor.substr(0, ANCHO_VALOR)   // substr(inicio, cantidad)
                 << "... (" << bins[i].valor.size() << " letras)";
        } else {
            cout << bins[i].valor;
        }
        cout << endl;
    }
}

int TablaHash::getNumKeys() {
    return num_keys;
}

int TablaHash::getColisiones() {
    return colisiones;
}

int TablaHash::getLength() {
    return bins.size();
}

double TablaHash::getFactorCarga()  {
    return (double) num_keys/getLength();
}

int TablaHash::getRedimensionamientos() {
    return redimensionamientos;
}

bool TablaHash::esPrimo(int n) {
    if (n < 2) return false;

    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }

    return true;
}

int TablaHash::siguientePrimo(int n) {
    while (!esPrimo(n)) {
        n++;
    }

    return n;
}

const vector<Bin>& TablaHash::getBins() {
    return bins;
}
