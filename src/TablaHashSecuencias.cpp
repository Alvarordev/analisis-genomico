// =============================================================================
// TablaHashSecuencias.cpp  —  PARTE B: implementación de la tabla hash 1
// =============================================================================
#include "TablaHashSecuencias.h"

#include <iomanip>   // setw: para imprimir columnas alineadas
#include <iostream>

using namespace std;

const int TAMANO_INICIAL = 10;  // "const" es como "final" en Java

// Constructor. Lo que va después de ":" inicializa los atributos antes de
// entrar al cuerpo. tabla(TAMANO_INICIAL) crea un vector con 10 casillas.
TablaHashSecuencias::TablaHashSecuencias()
    : tabla(TAMANO_INICIAL), cantidad(0), colisiones(0), redimensionamientos(0) {
}

// -----------------------------------------------------------------------------
// Función hash polinomial: trata el texto como un número en base 31.
//   hash("AB") = 'A' * 31 + 'B'
// Usamos "unsigned long long" (entero sin signo de 64 bits) para que el número
// no se vuelva negativo si es muy grande. Si se desborda, simplemente "da la
// vuelta", lo cual no es problema para un hash.
// -----------------------------------------------------------------------------
int TablaHashSecuencias::funcionHash(string clave) {
    unsigned long long hash = 0;
    for (char c : clave) {
        hash = hash * 31 + c;
    }
    return hash % tabla.size();  // Lo dejamos dentro del rango [0, tamaño-1]
}

void TablaHashSecuencias::insertar(SecuenciaADN nueva) {
    // 1) Si el ID ya existe, solo actualizamos su secuencia.
    SecuenciaADN* existente = buscar(nueva.id);
    if (existente != nullptr) {
        // Con punteros se usa "->" en vez de "." para acceder a los atributos.
        existente->secuencia = nueva.secuencia;
        return;
    }

    // 2) Es un elemento nuevo: revisamos si el factor de carga pasaría de 1.
    //    (cantidad + 1) / tamaño > 1   equivale a   cantidad + 1 > tamaño
    if (cantidad + 1 > getTamano()) {
        int nuevoTamano = siguientePrimo(2 * (cantidad + 1));  // factor ~0.5
        redimensionar(nuevoTamano);
    }

    // 3) Insertamos.
    insertarSinRedimensionar(nueva);
}

void TablaHashSecuencias::insertarSinRedimensionar(SecuenciaADN nueva) {
    int pos = funcionHash(nueva.id);

    // Linear probing: mientras la casilla esté ocupada, pasamos a la siguiente.
    // El "% getTamano()" hace que después de la última casilla volvamos a la 0.
    while (tabla[pos].ocupada) {
        colisiones++;
        pos = (pos + 1) % getTamano();
    }

    tabla[pos].dato = nueva;
    tabla[pos].ocupada = true;
    cantidad++;
}

void TablaHashSecuencias::redimensionar(int nuevoTamano) {
    // Guardamos una copia de la tabla vieja.
    // (En C++, asignar un vector COPIA todos sus elementos; en Java solo se
    //  copiaría la referencia.)
    vector<CasillaSecuencia> tablaVieja = tabla;

    // Creamos una tabla nueva y vacía con el nuevo tamaño.
    tabla = vector<CasillaSecuencia>(nuevoTamano);
    cantidad = 0;

    // Reinsertamos cada elemento. Hay que recalcular su posición porque
    // la función hash depende del tamaño de la tabla.
    for (CasillaSecuencia casilla : tablaVieja) {
        if (casilla.ocupada) {
            insertarSinRedimensionar(casilla.dato);
        }
    }
    redimensionamientos++;
}

SecuenciaADN* TablaHashSecuencias::buscar(string id) {
    int pos = funcionHash(id);

    // Recorremos como máximo toda la tabla, igual que al insertar.
    for (int i = 0; i < getTamano(); i++) {
        // Si llegamos a una casilla vacía, el ID no está en la tabla.
        if (!tabla[pos].ocupada) {
            return nullptr;
        }
        if (tabla[pos].dato.id == id) {  // En C++ los strings se comparan con ==
            // "&" delante de una variable obtiene su dirección (un puntero).
            return &tabla[pos].dato;
        }
        pos = (pos + 1) % getTamano();
    }
    return nullptr;
}

void TablaHashSecuencias::imprimir() {
    cout << "Tabla hash 1 (linear probing)" << endl;
    cout << "  Elementos: " << cantidad << "  |  Tamano: " << getTamano()
         << "  |  Factor de carga: " << fixed << setprecision(3) << getFactorCarga()
         << "  |  Colisiones: " << colisiones
         << "  |  Redimensionamientos: " << redimensionamientos << endl;

    // setw(n) reserva n espacios para el siguiente dato; left lo alinea a la izquierda.
    cout << left << setw(6) << "Bin" << setw(30) << "Key (ID secuencia)" << "Valor" << endl;

    for (int i = 0; i < getTamano(); i++) {
        cout << left << setw(6) << i;
        if (!tabla[i].ocupada) {
            cout << "-" << endl;
            continue;
        }
        SecuenciaADN s = tabla[i].dato;
        cout << setw(30) << s.id;
        // Mostramos solo los primeros 40 caracteres para no llenar la pantalla.
        if (s.secuencia.size() > 40) {
            cout << s.secuencia.substr(0, 40) << "... (" << s.secuencia.size() << " bases)";
        } else {
            cout << s.secuencia;
        }
        cout << endl;
    }
}

int TablaHashSecuencias::getCantidad() { return cantidad; }
int TablaHashSecuencias::getTamano() { return tabla.size(); }
int TablaHashSecuencias::getColisiones() { return colisiones; }
int TablaHashSecuencias::getRedimensionamientos() { return redimensionamientos; }

double TablaHashSecuencias::getFactorCarga() {
    // Convertimos a double para que la división no sea entera (igual que en Java).
    return (double) cantidad / getTamano();
}

// -----------------------------------------------------------------------------
// Números primos
// -----------------------------------------------------------------------------
bool TablaHashSecuencias::esPrimo(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; d++) {
        if (n % d == 0) return false;
    }
    return true;
}

int TablaHashSecuencias::siguientePrimo(int n) {
    while (!esPrimo(n)) {
        n++;
    }
    return n;
}
