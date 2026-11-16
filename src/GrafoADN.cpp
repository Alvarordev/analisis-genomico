// =============================================================================
// GrafoADN.cpp  —  PARTE D: implementación del grafo de 2-mers
// =============================================================================
#include "GrafoADN.h"

#include <cmath>     // pow: potencia (como Math.pow en Java)
#include <iomanip>
#include <iostream>

using namespace std;

GrafoADN::GrafoADN() : beta(0) {
}

GrafoADN::GrafoADN(string secuencia, double beta) : beta(beta) {
    // ---- Paso 1: partir la secuencia en pares sin solaparse ----
    // Avanzamos de 2 en 2. Si el largo es impar, la última letra sobra.
    for (int i = 0; i + 2 <= (int) secuencia.size(); i += 2) {
        pares.push_back(secuencia.substr(i, 2));
    }

    // ---- Paso 2: crear los vértices (pares distintos) ----
    // Guardamos en "indices" el número de vértice que le toca a cada posición,
    // para no tener que buscarlo otra vez dentro del doble for.
    vector<int> indices;
    for (string par : pares) {
        indices.push_back(agregarVertice(par));
    }

    // Matriz n x n llena de ceros.
    // vector<double>(n, 0.0) crea una fila de n ceros; la repetimos n veces.
    int n = vertices.size();
    matriz = vector<vector<double>>(n, vector<double>(n, 0.0));

    // ---- Pasos 3 y 4: arcos dirigidos con su peso ----
    // Las posiciones del enunciado empiezan en 1 y las del vector en 0, pero
    // como solo usamos la diferencia (j - i), el resultado es el mismo.
    int cantidadPares = pares.size();
    for (int i = 0; i < cantidadPares; i++) {
        for (int j = i + 1; j < cantidadPares; j++) {
            double peso = 1.0 / pow(j - i, beta);
            int u = indices[i];
            int v = indices[j];
            matriz[u][v] += peso;  // Si el arco ya existía, se suma
        }
    }
}

int GrafoADN::indiceDe(string par) {
    // Búsqueda lineal. Hay a lo más 16 vértices, así que es muy rápida.
    for (int i = 0; i < (int) vertices.size(); i++) {
        if (vertices[i] == par) {
            return i;
        }
    }
    return -1;
}

int GrafoADN::agregarVertice(string par) {
    int indice = indiceDe(par);
    if (indice == -1) {
        vertices.push_back(par);
        indice = vertices.size() - 1;
    }
    return indice;
}

double GrafoADN::getPeso(string u, string v) {
    int a = indiceDe(u);
    int b = indiceDe(v);
    if (a == -1 || b == -1) {
        return 0;
    }
    return matriz[a][b];
}

void GrafoADN::imprimirPares() {
    cout << left << setw(10) << "Posicion" << "2-mer" << endl;
    for (int i = 0; i < (int) pares.size(); i++) {
        cout << left << setw(10) << (i + 1) << pares[i] << endl;
    }
}

void GrafoADN::imprimirMatriz() {
    int n = vertices.size();
    // fixed + setprecision(n) muestra n decimales. Este formato se queda
    // activo para todo lo que se imprima después con cout.
    cout << fixed << setprecision(2);
    cout << "Matriz de adyacencia (" << n << " x " << n << "), beta = " << beta << endl;
    cout << "Fila = vertice de origen (u), columna = vertice de destino (v)" << endl;

    // Encabezado con los nombres de las columnas.
    cout << setw(6) << "";
    for (string v : vertices) {
        cout << right << setw(9) << v;
    }
    cout << endl;

    // Una fila por vértice, con 4 decimales.
    cout << setprecision(4);
    for (int a = 0; a < n; a++) {
        cout << left << setw(6) << vertices[a];
        for (int b = 0; b < n; b++) {
            cout << right << setw(9) << matriz[a][b];
        }
        cout << endl;
    }
}

int GrafoADN::getCantidadVertices() { return vertices.size(); }
int GrafoADN::getCantidadPares() { return pares.size(); }
double GrafoADN::getBeta() { return beta; }
