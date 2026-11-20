#include "ModoGUI.h"
#include "../doubleHashing/TablaKmers.h"
#include "../grafo/GrafoADN.h"
#include "../linearProbing/TablaHash.h"
#include "../maxHeap/MaxHeap.h"
#include "../parser/LectorFasta.h"
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static const int TAMANO_INICIAL = 10;
static const int K_MINIMO = 1;
static const int K_MAXIMO = 10;
static const int LARGO_VISTA_PREVIA = 60;
static const int MAXIMO_PARES = 200;

static string jsonTexto(string texto) {
    string resultado = "\"";

    for (char c : texto) {
        if (c == '"') {
            resultado += "\\\"";
        } else if (c == '\\') {
            resultado += "\\\\";
        } else if (c == '\n') {
            resultado += "\\n";
        } else if (c == '\t') {
            resultado += "\\t";
        } else if ((unsigned char) c < 0x20) {
            resultado += ' ';
        } else {
            resultado += c;
        }
    }

    return resultado + "\"";
}

static string jsonNumero(double x) {
    ostringstream s;
    s.precision(10);
    s << x;
    return s.str();
}

static void responderError(string mensaje) {
    cout << "{\"ok\":false,\"error\":" << jsonTexto(mensaje) << "}" << endl;
}

static vector<string> separar(string linea) {
    vector<string> partes;
    string actual;

    for (char c : linea) {
        if (c == '\t') {
            partes.push_back(actual);
            actual = "";
        } else if (c != '\r') {
            actual += c;
        }
    }
    partes.push_back(actual);

    return partes;
}

static string aMayusculas(string texto) {
    for (char& c : texto) {
        c = toupper((unsigned char) c);
    }

    return texto;
}

static bool aEntero(string texto, int& resultado) {
    try {
        size_t usados = 0;
        resultado = stoi(texto, &usados);
        return usados == texto.size();
    } catch (...) {
        return false;
    }
}

static bool aDecimal(string texto, double& resultado) {
    try {
        size_t usados = 0;
        resultado = stod(texto, &usados);
        return usados == texto.size() && isfinite(resultado);
    } catch (...) {
        return false;
    }
}

static string estadisticasTabla1(TablaHash& tabla) {
    ostringstream s;
    s << "\"cantidad\":" << tabla.getNumKeys()
      << ",\"tamano\":" << tabla.getLength()
      << ",\"factorCarga\":" << jsonNumero(tabla.getFactorCarga())
      << ",\"colisiones\":" << tabla.getColisiones()
      << ",\"redimensionamientos\":" << tabla.getRedimensionamientos();
    return s.str();
}

static string estadisticasTabla2(TablaKmers& tabla, string id) {
    ostringstream s;
    s << "\"id\":" << jsonTexto(id)
      << ",\"k\":" << tabla.getK()
      << ",\"tamano\":" << tabla.getLength()
      << ",\"distintos\":" << tabla.getDistintos()
      << ",\"totales\":" << tabla.getTotales()
      << ",\"factorCarga\":" << jsonNumero(tabla.getFactorCarga())
      << ",\"colisiones\":" << tabla.getColisiones();
    return s.str();
}

static void responderCargar(vector<SecuenciaADN>& secuencias, string ruta, TablaHash& tabla) {
    cout << "{\"ok\":true,\"archivo\":" << jsonTexto(ruta)
         << ",\"leidas\":" << secuencias.size() << "," << estadisticasTabla1(tabla)
         << ",\"ids\":[";
    for (int i = 0; i < (int) secuencias.size(); i++) {
        if (i > 0) {
            cout << ",";
        }
        cout << jsonTexto(secuencias[i].id);
    }
    cout << "]}" << endl;
}

static void responderTabla1(TablaHash& tabla) {
    const vector<Bin>& bins = tabla.getBins();

    cout << "{\"ok\":true," << estadisticasTabla1(tabla) << ",\"casillas\":[";
    for (int i = 0; i < (int) bins.size(); i++) {
        if (i > 0) {
            cout << ",";
        }
        if (bins[i].estado != OCUPADO) {
            cout << "null";
            continue;
        }
        cout << "{\"id\":" << jsonTexto(bins[i].key)
             << ",\"largo\":" << bins[i].valor.size()
             << ",\"inicio\":" << jsonTexto(bins[i].valor.substr(0, LARGO_VISTA_PREVIA)) << "}";
    }
    cout << "]}" << endl;
}

static void responderTabla2(TablaKmers& tabla, string id) {
    const vector<KmerConteo>& bins = tabla.getBins();
    bool primero = true;

    cout << "{\"ok\":true," << estadisticasTabla2(tabla, id) << ",\"casillas\":[";
    for (int i = 0; i < (int) bins.size(); i++) {
        if (bins[i].conteo == 0) {
            continue;
        }
        if (!primero) {
            cout << ",";
        }
        primero = false;
        cout << "{\"bin\":" << i << ",\"kmer\":" << jsonTexto(bins[i].kmer)
             << ",\"conteo\":" << bins[i].conteo << "}";
    }
    cout << "]}" << endl;
}

static void responderTop(TablaKmers& tabla, int n) {
    MaxHeap heap(tabla.obtenerKmers());
    vector<KmerConteo> mejores = heap.topN(n);

    cout << "{\"ok\":true,\"k\":" << tabla.getK()
         << ",\"distintos\":" << heap.getLength() << ",\"top\":[";
    for (int i = 0; i < (int) mejores.size(); i++) {
        if (i > 0) {
            cout << ",";
        }
        cout << "{\"kmer\":" << jsonTexto(mejores[i].kmer)
             << ",\"conteo\":" << mejores[i].conteo << "}";
    }
    cout << "]}" << endl;
}

static void responderGrafo(GrafoADN& grafo, string origen) {
    const vector<string>& pares = grafo.getPares();
    const vector<string>& vertices = grafo.getVertices();
    const vector<vector<double>>& matriz = grafo.getMatriz();

    cout << "{\"ok\":true,\"origen\":" << jsonTexto(origen)
         << ",\"beta\":" << jsonNumero(grafo.getBeta())
         << ",\"cantidadPares\":" << pares.size() << ",\"pares\":[";
    if ((int) pares.size() <= MAXIMO_PARES) {
        for (int i = 0; i < (int) pares.size(); i++) {
            if (i > 0) {
                cout << ",";
            }
            cout << "{\"par\":" << jsonTexto(pares[i])
                 << ",\"valido\":" << (GrafoADN::esParValido(pares[i]) ? "true" : "false") << "}";
        }
    }

    cout << "],\"vertices\":[";
    for (int i = 0; i < (int) vertices.size(); i++) {
        if (i > 0) {
            cout << ",";
        }
        cout << jsonTexto(vertices[i]);
    }

    cout << "],\"matriz\":[";
    for (int a = 0; a < (int) matriz.size(); a++) {
        if (a > 0) {
            cout << ",";
        }
        cout << "[";
        for (int b = 0; b < (int) matriz[a].size(); b++) {
            if (b > 0) {
                cout << ",";
            }
            cout << jsonNumero(matriz[a][b]);
        }
        cout << "]";
    }
    cout << "]}" << endl;
}

void ejecutarModoGUI() {
    TablaHash tablaSecuencias(TAMANO_INICIAL);
    TablaKmers tablaKmers(K_MINIMO);
    string idSecuenciaKmers = "";
    bool hayArchivo = false;
    bool hayTablaKmers = false;

    string linea;
    while (getline(cin, linea)) {
        vector<string> p = separar(linea);
        string comando = p[0];

        if (comando == "salir") {
            cout << "{\"ok\":true}" << endl;
            break;
        }

        if (comando == "cargar") {
            if (p.size() < 2 || p[1].empty()) {
                responderError("Falta la ruta del archivo.");
                continue;
            }

            vector<SecuenciaADN> secuencias = LectorFasta::leerArchivo(p[1]);
            if (secuencias.empty()) {
                responderError("No se pudieron leer secuencias de " + p[1]);
                continue;
            }

            tablaSecuencias = TablaHash(TAMANO_INICIAL);
            for (SecuenciaADN s : secuencias) {
                tablaSecuencias.insertar(s.id, s.secuencia);
            }
            hayArchivo = true;
            hayTablaKmers = false;
            idSecuenciaKmers = "";

            responderCargar(secuencias, p[1], tablaSecuencias);
            continue;
        }

        if (comando == "grafo") {
            double beta;
            if (p.size() < 3 || !aDecimal(p[1], beta)) {
                responderError("Beta no es un número válido.");
                continue;
            }

            string secuencia = aMayusculas(p[2]);
            string origen = "";
            string* encontrada = tablaSecuencias.buscar(p[2]);
            if (encontrada != nullptr) {
                secuencia = *encontrada;
                origen = p[2];
            }

            if (secuencia.size() < 2) {
                responderError("No existe ese ID y el texto no es una secuencia de al menos 2 bases.");
                continue;
            }

            GrafoADN grafo(secuencia, beta);
            responderGrafo(grafo, origen);
            continue;
        }

        if (comando != "tabla1" && comando != "buscar" && comando != "kmers" &&
            comando != "tabla2" && comando != "kmer" && comando != "top") {
            responderError("Comando desconocido: " + comando);
            continue;
        }

        if (!hayArchivo) {
            responderError("Primero carga un archivo FASTA.");
            continue;
        }

        if (comando == "tabla1") {
            responderTabla1(tablaSecuencias);
            continue;
        }

        if (comando == "buscar") {
            string id = p.size() > 1 ? p[1] : "";
            string* secuencia = tablaSecuencias.buscar(id);
            if (secuencia == nullptr) {
                responderError("No existe una secuencia con ID '" + id + "'.");
                continue;
            }

            cout << "{\"ok\":true,\"id\":" << jsonTexto(id)
                 << ",\"largo\":" << secuencia->size()
                 << ",\"secuencia\":" << jsonTexto(*secuencia) << "}" << endl;
            continue;
        }

        if (comando == "kmers") {
            if (p.size() < 3) {
                responderError("Uso: kmers<TAB>id<TAB>k");
                continue;
            }

            string* secuencia = tablaSecuencias.buscar(p[1]);
            if (secuencia == nullptr) {
                responderError("No existe una secuencia con ID '" + p[1] + "'.");
                continue;
            }

            int k;
            if (!aEntero(p[2], k) || k < K_MINIMO || k > K_MAXIMO) {
                responderError("k debe estar entre " + to_string(K_MINIMO) + " y " + to_string(K_MAXIMO) + ".");
                continue;
            }

            tablaKmers = TablaKmers(k);
            tablaKmers.construirDesde(*secuencia);
            idSecuenciaKmers = p[1];
            hayTablaKmers = true;

            cout << "{\"ok\":true," << estadisticasTabla2(tablaKmers, idSecuenciaKmers) << "}" << endl;
            continue;
        }

        if (!hayTablaKmers) {
            responderError("Primero construye la tabla de k-mers.");
            continue;
        }

        if (comando == "tabla2") {
            responderTabla2(tablaKmers, idSecuenciaKmers);
            continue;
        }

        if (comando == "kmer") {
            string kmer = aMayusculas(p.size() > 1 ? p[1] : "");
            if ((int) kmer.size() != tablaKmers.getK()) {
                responderError("El k-mer debe tener " + to_string(tablaKmers.getK()) + " letras.");
                continue;
            }

            cout << "{\"ok\":true,\"kmer\":" << jsonTexto(kmer)
                 << ",\"conteo\":" << tablaKmers.buscar(kmer) << "}" << endl;
            continue;
        }

        if (comando == "top") {
            int n;
            if (p.size() < 2 || !aEntero(p[1], n) || n <= 0) {
                responderError("n debe ser un número mayor que 0.");
                continue;
            }

            responderTop(tablaKmers, n);
        }
    }
}
