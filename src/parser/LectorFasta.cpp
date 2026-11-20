#include "LectorFasta.h"
#include <cctype>
#include <fstream>
#include <iostream>

using namespace std;

vector<SecuenciaADN> LectorFasta::leerArchivo(string ruta) {
    vector<SecuenciaADN> secuencias;

    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << ruta << endl;
        return secuencias;
    }

    SecuenciaADN actual;
    bool hayRegistro = false;
    string linea;

    while (getline(archivo, linea)) {
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }

        if (linea.empty() || linea[0] == ';') {
            continue;
        }

        if (linea[0] == '>') {
            if (hayRegistro) {
                secuencias.push_back(actual);
            }

            string encabezado = linea.substr(1);
            size_t posEspacio = encabezado.find(' ');
            if (posEspacio != string::npos) {
                encabezado = encabezado.substr(0, posEspacio);
            }

            actual.id = encabezado;
            actual.secuencia = "";
            hayRegistro = true;
        } else if (hayRegistro) {
            actual.secuencia += limpiarLinea(linea);
        }
    }

    if (hayRegistro) {
        secuencias.push_back(actual);
    }

    return secuencias;
}

// Solo se guardan letras: se descartan espacios, gaps ('-', '.') y números.
// Las letras ambiguas (N, R, Y...) se conservan y la tabla 2 y el grafo las saltan.
string LectorFasta::limpiarLinea(string linea) {
    string resultado = "";

    for (char c : linea) {
        if (isalpha((unsigned char) c)) {
            resultado += (char) toupper((unsigned char) c);
        }
    }

    return resultado;
}
