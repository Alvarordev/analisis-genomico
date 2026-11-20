#include "GrafoADN.h"
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace std;

GrafoADN::GrafoADN(string secuencia, double beta): beta(beta) {
    for (int i = 0; i + 2 <= (int) secuencia.size(); i += 2) {
        pares.push_back(secuencia.substr(i, 2));
    }

    // Un par con letras que no son ACGT no es vértice (-1), pero su posición
    // se sigue contando para que j - i sea la distancia real.
    vector<int> indices;
    for (string par : pares) {
        if (esParValido(par)) {
            indices.push_back(agregarVertice(par));
        } else {
            indices.push_back(-1);
        }
    }

    int n = vertices.size();
    matriz = vector<vector<double>>(n, vector<double>(n, 0.0));

    int cantidadPares = pares.size();
    for (int i = 0; i < cantidadPares; i++) {
        if (indices[i] == -1) {
            continue;
        }

        for (int j = i + 1; j < cantidadPares; j++) {
            if (indices[j] == -1) {
                continue;
            }

            double peso = 1.0 / pow(j - i, beta);
            matriz[indices[i]][indices[j]] += peso;
        }
    }
}

bool GrafoADN::esParValido(string par) {
    for (char c : par) {
        if (c != 'A' && c != 'C' && c != 'G' && c != 'T') {
            return false;
        }
    }

    return true;
}

int GrafoADN::indiceDe(string par) {
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
        cout << left << setw(10) << (i + 1) << pares[i];
        if (!esParValido(pares[i])) {
            cout << "   (se salta: no es A/C/G/T)";
        }
        cout << endl;
    }
}

void GrafoADN::imprimirMatriz() {
    int n = vertices.size();

    cout << fixed << setprecision(2);
    cout << "Matriz de adyacencia (" << n << " x " << n << "), beta = " << beta << endl;
    cout << "Fila = vertice de origen (u), columna = vertice de destino (v)" << endl;

    cout << setw(6) << "";
    for (string v : vertices) {
        cout << right << setw(9) << v;
    }
    cout << endl;

    cout << setprecision(4);
    for (int a = 0; a < n; a++) {
        cout << left << setw(6) << vertices[a];
        for (int b = 0; b < n; b++) {
            cout << right << setw(9) << matriz[a][b];
        }
        cout << endl;
    }
}

int GrafoADN::getCantidadVertices() {
    return vertices.size();
}

int GrafoADN::getCantidadPares() {
    return pares.size();
}

double GrafoADN::getBeta() {
    return beta;
}

const vector<string>& GrafoADN::getPares() {
    return pares;
}

const vector<string>& GrafoADN::getVertices() {
    return vertices;
}

const vector<vector<double>>& GrafoADN::getMatriz() {
    return matriz;
}
