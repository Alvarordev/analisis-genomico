// =============================================================================
// LectorFasta.cpp  —  PARTE A: implementación del parser
// =============================================================================
#include "LectorFasta.h"

#include <cctype>    // toupper (pasar a mayúsculas) e isspace (es espacio)
#include <fstream>   // ifstream: para leer archivos (como Scanner/FileReader)
#include <iostream>  // cout y cerr: para imprimir en pantalla

// "using namespace std" permite escribir string en vez de std::string,
// vector en vez de std::vector, etc. Lo usamos solo en los .cpp.
using namespace std;

// Así se implementa un método fuera de la clase: NombreClase::nombreMetodo.
// (La palabra "static" se pone solo en el .h, no aquí.)
vector<SecuenciaADN> LectorFasta::leerArchivo(string ruta) {
    vector<SecuenciaADN> secuencias;  // Lista vacía (como new ArrayList<>())

    ifstream archivo(ruta);  // Abre el archivo para leer
    if (!archivo.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << ruta << endl;
        return secuencias;  // Devuelve la lista vacía
    }

    // En C++ un objeto se puede crear sin "new". Esta variable ya es un
    // SecuenciaADN con id y secuencia vacíos.
    SecuenciaADN actual;
    bool hayRegistro = false;  // ¿Ya encontramos el primer '>'?
    string linea;

    // getline lee una línea completa. Devuelve false al llegar al final.
    while (getline(archivo, linea)) {
        // Los archivos creados en Windows terminan cada línea con '\r'.
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();  // Quita el último carácter
        }

        // Saltamos las líneas vacías y los comentarios (empiezan con ';').
        if (linea.empty() || linea[0] == ';') {
            continue;
        }

        if (linea[0] == '>') {
            // Empieza una secuencia nueva: guardamos la anterior, si había.
            if (hayRegistro) {
                secuencias.push_back(actual);  // push_back = add() de Java
            }

            // El ID es el texto que viene después de '>' y antes del primer
            // espacio. Lo que sigue (si hay) es una descripción y se ignora.
            string encabezado = linea.substr(1);  // Quita el '>'
            int posEspacio = encabezado.find(' ');
            if (posEspacio != (int) string::npos) {  // npos = "no encontrado"
                encabezado = encabezado.substr(0, posEspacio);
            }

            actual.id = encabezado;
            actual.secuencia = "";
            hayRegistro = true;
        } else if (hayRegistro) {
            // Es una línea de secuencia: la limpiamos y la agregamos al final.
            actual.secuencia += limpiarLinea(linea);
        }
    }

    // La última secuencia no tiene un '>' después, así que la guardamos aquí.
    if (hayRegistro) {
        secuencias.push_back(actual);
    }

    // No hace falta cerrar el archivo: se cierra solo al terminar la función.
    return secuencias;
}

string LectorFasta::limpiarLinea(string linea) {
    string resultado = "";
    // "for (char c : linea)" es el for-each de Java: for (char c : texto)
    for (char c : linea) {
        if (isspace(c) || c == '-' || c == '.') {
            continue;  // Ignoramos espacios y gaps de alineamiento
        }
        resultado += (char) toupper(c);  // Guardamos la letra en mayúscula
    }
    return resultado;
}
