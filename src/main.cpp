#include "doubleHashing/TablaKmers.h"
#include "grafo/GrafoADN.h"
#include "linearProbing/TablaHash.h"
#include "maxHeap/MaxHeap.h"
#include "modoGUI/ModoGUI.h"
#include "parser/LectorFasta.h"
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

const string ARCHIVO_POR_DEFECTO = "data/HIV2_ALL_2021_env_DNA.2000.fasta";
const int TAMANO_INICIAL = 10;
const int K_MINIMO = 1;
const int K_MAXIMO = 10;

string leerTexto(string mensaje) {
    cout << mensaje;
    string linea;
    getline(cin, linea);
    return linea;
}

int leerEntero(string mensaje) {
    string linea = leerTexto(mensaje);

    try {
        return stoi(linea);
    } catch (...) {
        return -1;
    }
}

string aMayusculas(string texto) {
    for (char& c : texto) {
        c = toupper((unsigned char) c);
    }

    return texto;
}

bool cargarArchivo(TablaHash& tablaSecuencias, string ruta) {
    vector<SecuenciaADN> secuencias = LectorFasta::leerArchivo(ruta);

    if (secuencias.empty()) {
        cout << "No se cargaron secuencias." << endl;
        return false;
    }

    tablaSecuencias = TablaHash(TAMANO_INICIAL);
    for (SecuenciaADN s : secuencias) {
        tablaSecuencias.insertar(s.id, s.secuencia);
    }

    cout << "Se leyeron " << secuencias.size() << " secuencias de " << ruta << endl;
    cout << "Tabla 1: " << tablaSecuencias.getNumKeys() << " keys en "
         << tablaSecuencias.getLength() << " bins (factor de carga "
         << tablaSecuencias.getFactorCarga() << ", "
         << tablaSecuencias.getRedimensionamientos() << " redimensionamientos)" << endl;
    return true;
}

void buscarSecuencia(TablaHash& tablaSecuencias) {
    string id = leerTexto("ID de la secuencia: ");
    string* secuencia = tablaSecuencias.buscar(id);

    if (secuencia == nullptr) {
        cout << "No existe una secuencia con ID '" << id << "'." << endl;
        return;
    }

    cout << ">" << id << " (" << secuencia->size() << " bases)" << endl;
    cout << *secuencia << endl;
}

string construirTablaKmers(TablaHash& tablaSecuencias, TablaKmers& tablaKmers) {
    string id = leerTexto("ID de la secuencia: ");
    string* secuencia = tablaSecuencias.buscar(id);

    if (secuencia == nullptr) {
        cout << "No existe una secuencia con ID '" << id << "'." << endl;
        return "";
    }

    int k = leerEntero("Valor de k (" + to_string(K_MINIMO) + " a " + to_string(K_MAXIMO) + "): ");
    if (k < K_MINIMO || k > K_MAXIMO) {
        cout << "k debe estar entre " << K_MINIMO << " y " << K_MAXIMO << "." << endl;
        return "";
    }

    tablaKmers = TablaKmers(k);
    tablaKmers.construirDesde(*secuencia);

    cout << "Tabla 2 creada: " << tablaKmers.getTotales() << " " << k << "-mers ("
         << tablaKmers.getDistintos() << " distintos) en "
         << tablaKmers.getLength() << " bins, "
         << tablaKmers.getColisiones() << " colisiones." << endl;
    return id;
}

void buscarKmer(TablaKmers& tablaKmers) {
    string kmer = aMayusculas(leerTexto("k-mer a buscar: "));

    if ((int) kmer.size() != tablaKmers.getK()) {
        cout << "El k-mer debe tener " << tablaKmers.getK() << " letras." << endl;
        return;
    }

    cout << kmer << " aparece " << tablaKmers.buscar(kmer) << " veces." << endl;
}

void mostrarMasFrecuentes(TablaKmers& tablaKmers) {
    int n = leerEntero("Cuantos k-mers quieres ver (n): ");

    if (n <= 0) {
        cout << "n debe ser un numero mayor que 0." << endl;
        return;
    }

    MaxHeap heap(tablaKmers.obtenerKmers());
    vector<KmerConteo> mejores = heap.topN(n);

    cout << "Los " << mejores.size() << " " << tablaKmers.getK()
         << "-mers mas frecuentes (de " << heap.getLength() << " distintos):" << endl;
    for (int i = 0; i < (int) mejores.size(); i++) {
        cout << "  " << (i + 1) << ". " << mejores[i].kmer
             << "  ->  " << mejores[i].conteo << endl;
    }
}

void mostrarGrafo(TablaHash& tablaSecuencias) {
    string texto = leerTexto("ID de la secuencia (o escribe una secuencia de ADN): ");

    string secuencia;
    string* encontrada = tablaSecuencias.buscar(texto);
    if (encontrada != nullptr) {
        secuencia = *encontrada;
    } else {
        secuencia = aMayusculas(texto);
    }

    if (secuencia.size() < 2) {
        cout << "La secuencia debe tener al menos 2 bases." << endl;
        return;
    }

    string textoBeta = leerTexto("Valor de beta [1]: ");
    double beta = 1.0;
    if (!textoBeta.empty()) {
        try {
            beta = stod(textoBeta);
        } catch (...) {
            cout << "Beta no valido." << endl;
            return;
        }
    }

    GrafoADN grafo(secuencia, beta);

    if (grafo.getCantidadPares() <= 30) {
        grafo.imprimirPares();
        cout << endl;
    } else {
        cout << "Pares de nucleotidos: " << grafo.getCantidadPares() << endl;
    }
    grafo.imprimirMatriz();
}

void mostrarMenu() {
    cout << endl;
    cout << "===== Mini sistema de analisis genomico =====" << endl;
    cout << " 1. Cargar archivo FASTA (parser + tabla hash 1)" << endl;
    cout << " 2. Mostrar tabla hash 1 (secuencias)" << endl;
    cout << " 3. Buscar secuencia por ID" << endl;
    cout << " 4. Construir tabla hash 2 (k-mers de una secuencia)" << endl;
    cout << " 5. Mostrar tabla hash 2" << endl;
    cout << " 6. Buscar un k-mer" << endl;
    cout << " 7. Ver los n k-mers mas frecuentes (max-heap)" << endl;
    cout << " 8. Grafo de 2-mers y matriz de adyacencia" << endl;
    cout << " 0. Salir" << endl;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "--gui") {
        ejecutarModoGUI();
        return 0;
    }

    TablaHash tablaSecuencias(TAMANO_INICIAL);
    TablaKmers tablaKmers(K_MINIMO);
    string idSecuenciaKmers = "";
    bool hayArchivo = false;
    bool hayTablaKmers = false;

    if (argc > 1) {
        hayArchivo = cargarArchivo(tablaSecuencias, argv[1]);
    }

    int opcion = -1;
    while (opcion != 0) {
        mostrarMenu();
        opcion = leerEntero("Opcion: ");

        if (cin.eof()) {
            opcion = 0;
        }

        if (opcion >= 2 && opcion <= 7 && !hayArchivo) {
            cout << "Primero carga un archivo FASTA (opcion 1)." << endl;
            continue;
        }

        if (opcion >= 5 && opcion <= 7 && !hayTablaKmers) {
            cout << "Primero construye la tabla de k-mers (opcion 4)." << endl;
            continue;
        }

        switch (opcion) {
            case 1: {
                string ruta = leerTexto("Ruta del archivo [" + ARCHIVO_POR_DEFECTO + "]: ");
                if (ruta.empty()) {
                    ruta = ARCHIVO_POR_DEFECTO;
                }
                if (cargarArchivo(tablaSecuencias, ruta)) {
                    hayArchivo = true;
                    hayTablaKmers = false;
                }
                break;
            }
            case 2:
                tablaSecuencias.imprimir();
                break;
            case 3:
                buscarSecuencia(tablaSecuencias);
                break;
            case 4: {
                string id = construirTablaKmers(tablaSecuencias, tablaKmers);
                if (id != "") {
                    idSecuenciaKmers = id;
                    hayTablaKmers = true;
                }
                break;
            }
            case 5:
                cout << "Secuencia: " << idSecuenciaKmers << endl;
                tablaKmers.imprimir();
                break;
            case 6:
                buscarKmer(tablaKmers);
                break;
            case 7:
                mostrarMasFrecuentes(tablaKmers);
                break;
            case 8:
                mostrarGrafo(tablaSecuencias);
                break;
            case 0:
                cout << "Hasta luego." << endl;
                break;
            default:
                cout << "Opcion no valida." << endl;
        }
    }

    return 0;
}
