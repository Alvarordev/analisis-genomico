// =============================================================================
// main.cpp  —  Menú principal
// =============================================================================
// PARTE E: interfaz de texto. Desde aquí se llaman todas las partes:
//   a) parser FASTA            -> LectorFasta
//   b) tablas hash 1 y 2       -> TablaHashSecuencias, TablaHashKmers
//   c) max-heap                -> MaxHeap
//   d) grafo de 2-mers         -> GrafoADN
// =============================================================================
#include <iostream>
#include <string>
#include <vector>

#include "GrafoADN.h"
#include "LectorFasta.h"
#include "MaxHeap.h"
#include "SecuenciaADN.h"
#include "TablaHashKmers.h"
#include "TablaHashSecuencias.h"

using namespace std;

const string ARCHIVO_POR_DEFECTO = "data/muestra.fasta";
const int K_MINIMO = 1;
const int K_MAXIMO = 10;  // 4^10 = 1,048,576 casillas; más sería demasiada memoria

// -----------------------------------------------------------------------------
// Funciones para leer datos del teclado
// -----------------------------------------------------------------------------

// Muestra un mensaje y lee una línea completa (como scanner.nextLine()).
string leerTexto(string mensaje) {
    cout << mensaje;
    string linea;
    getline(cin, linea);
    return linea;
}

// Muestra un mensaje y lee un número entero. Devuelve -1 si no es un número.
int leerEntero(string mensaje) {
    string linea = leerTexto(mensaje);
    try {
        return stoi(linea);  // stoi = Integer.parseInt de Java
    } catch (...) {          // catch (...) atrapa cualquier excepción
        return -1;
    }
}

// Pasa un texto a mayúsculas (para que "atg" se busque como "ATG").
string aMayusculas(string texto) {
    for (int i = 0; i < (int) texto.size(); i++) {
        texto[i] = toupper(texto[i]);
    }
    return texto;
}

// -----------------------------------------------------------------------------
// Opciones del menú
// -----------------------------------------------------------------------------
// ¿Qué significa el "&" en "TablaHashSecuencias& tabla"?
//   En C++, si pasas un objeto a una función SIN "&", la función recibe una
//   COPIA, y los cambios no se ven afuera. Con "&" (paso por referencia) la
//   función trabaja con el objeto original, como pasa siempre en Java.
// -----------------------------------------------------------------------------

// Opción 1: leer el archivo FASTA (parte a) y llenar la tabla hash 1.
void cargarArchivo(TablaHashSecuencias& tablaSecuencias, string ruta) {
    vector<SecuenciaADN> secuencias = LectorFasta::leerArchivo(ruta);
    if (secuencias.empty()) {
        cout << "No se cargaron secuencias." << endl;
        return;
    }

    // Empezamos con una tabla nueva y vacía. En C++ no hace falta "new":
    // asignar un objeto nuevo reemplaza el anterior.
    tablaSecuencias = TablaHashSecuencias();

    for (SecuenciaADN s : secuencias) {
        tablaSecuencias.insertar(s);
    }

    cout << "Se leyeron " << secuencias.size() << " secuencias de " << ruta << endl;
    cout << "Tabla 1: " << tablaSecuencias.getCantidad() << " elementos en "
         << tablaSecuencias.getTamano() << " casillas (factor de carga "
         << tablaSecuencias.getFactorCarga() << ", "
         << tablaSecuencias.getRedimensionamientos() << " redimensionamientos)" << endl;
}

// Opción 3: buscar una secuencia por su ID.
void buscarSecuencia(TablaHashSecuencias& tablaSecuencias) {
    string id = leerTexto("ID de la secuencia: ");
    SecuenciaADN* encontrada = tablaSecuencias.buscar(id);

    if (encontrada == nullptr) {
        cout << "No existe una secuencia con ID '" << id << "'." << endl;
        return;
    }
    cout << ">" << encontrada->id << " (" << encontrada->secuencia.size() << " bases)" << endl;
    cout << encontrada->secuencia << endl;
}

// Opción 4: construir la tabla hash 2 con los k-mers de una secuencia.
// Devuelve el ID de la secuencia usada, o "" si hubo un error.
string construirTablaKmers(TablaHashSecuencias& tablaSecuencias, TablaHashKmers& tablaKmers) {
    string id = leerTexto("ID de la secuencia: ");
    SecuenciaADN* secuencia = tablaSecuencias.buscar(id);
    if (secuencia == nullptr) {
        cout << "No existe una secuencia con ID '" << id << "'." << endl;
        return "";
    }

    int k = leerEntero("Valor de k (" + to_string(K_MINIMO) + " a " + to_string(K_MAXIMO) + "): ");
    if (k < K_MINIMO || k > K_MAXIMO) {
        cout << "k debe estar entre " << K_MINIMO << " y " << K_MAXIMO << "." << endl;
        return "";
    }

    tablaKmers = TablaHashKmers(k);  // Tabla nueva de 4^k casillas
    tablaKmers.construirDesde(secuencia->secuencia);

    cout << "Tabla 2 creada: " << tablaKmers.getKmersTotales() << " " << k << "-mers ("
         << tablaKmers.getKmersDistintos() << " distintos) en "
         << tablaKmers.getTamano() << " casillas, "
         << tablaKmers.getColisiones() << " colisiones." << endl;
    return id;
}

// Opción 6: buscar cuántas veces aparece un k-mer.
void buscarKmer(TablaHashKmers& tablaKmers) {
    string kmer = aMayusculas(leerTexto("k-mer a buscar: "));
    if ((int) kmer.size() != tablaKmers.getK()) {
        cout << "El k-mer debe tener " << tablaKmers.getK() << " letras." << endl;
        return;
    }
    cout << kmer << " aparece " << tablaKmers.buscar(kmer) << " veces." << endl;
}

// Opción 7 (parte c): los n k-mers más frecuentes usando el max-heap.
void mostrarMasFrecuentes(TablaHashKmers& tablaKmers) {
    int n = leerEntero("Cuantos k-mers quieres ver (n): ");
    if (n <= 0) {
        cout << "n debe ser un numero mayor que 0." << endl;
        return;
    }

    // Se arma el heap con todos los pares (k-mer, conteo) de la tabla 2.
    MaxHeap heap(tablaKmers.obtenerKmers());
    vector<KmerConteo> mejores = heap.topN(n);

    cout << "Los " << mejores.size() << " " << tablaKmers.getK()
         << "-mers mas frecuentes (de " << heap.getTamano() << " distintos):" << endl;
    for (int i = 0; i < (int) mejores.size(); i++) {
        cout << "  " << (i + 1) << ". " << mejores[i].kmer
             << "  ->  " << mejores[i].conteo << endl;
    }
}

// Opción 8 (parte d): grafo de 2-mers y su matriz de adyacencia.
// Se puede usar una secuencia de la tabla 1 o escribir una a mano (por
// ejemplo la del enunciado: TCATTAGCAGTTAGCAGC).
void mostrarGrafo(TablaHashSecuencias& tablaSecuencias) {
    string texto = leerTexto("ID de la secuencia (o escribe una secuencia de ADN): ");

    string secuencia;
    SecuenciaADN* encontrada = tablaSecuencias.buscar(texto);
    if (encontrada != nullptr) {
        secuencia = encontrada->secuencia;
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
            beta = stod(textoBeta);  // stod = Double.parseDouble de Java
        } catch (...) {
            cout << "Beta no valido." << endl;
            return;
        }
    }

    GrafoADN grafo(secuencia, beta);

    // La tabla de pares solo se muestra si es corta, para no llenar la pantalla.
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

// -----------------------------------------------------------------------------
// main: el punto de entrada del programa (como public static void main).
// argc = cantidad de argumentos, argv = los argumentos (argv[0] es el programa).
// Ejemplo:  ./genomica data/muestra.fasta   ->   argv[1] = "data/muestra.fasta"
// -----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    // Los objetos se crean sin "new". Ya están listos para usarse.
    TablaHashSecuencias tablaSecuencias;
    TablaHashKmers tablaKmers;  // Vacía hasta que se use la opción 4
    string idSecuenciaKmers = "";

    bool hayArchivo = false;

    // Si se pasó un archivo al ejecutar el programa, lo cargamos al inicio.
    if (argc > 1) {
        cargarArchivo(tablaSecuencias, argv[1]);
        hayArchivo = tablaSecuencias.getCantidad() > 0;
    }

    int opcion = -1;
    while (opcion != 0) {
        mostrarMenu();
        opcion = leerEntero("Opcion: ");

        // Si se terminó la entrada (Ctrl+D), salimos.
        if (cin.eof()) {
            opcion = 0;
        }

        // Las opciones 2 a 7 necesitan que ya haya un archivo cargado.
        // (La 8 no, porque se puede escribir la secuencia a mano.)
        if (opcion >= 2 && opcion <= 7 && !hayArchivo) {
            cout << "Primero carga un archivo FASTA (opcion 1)." << endl;
            continue;
        }
        // Las opciones 5, 6 y 7 necesitan la tabla de k-mers.
        if (opcion >= 5 && opcion <= 7 && tablaKmers.getK() == 0) {
            cout << "Primero construye la tabla de k-mers (opcion 4)." << endl;
            continue;
        }

        switch (opcion) {
            case 1: {
                string ruta = leerTexto("Ruta del archivo [" + ARCHIVO_POR_DEFECTO + "]: ");
                if (ruta.empty()) {
                    ruta = ARCHIVO_POR_DEFECTO;
                }
                cargarArchivo(tablaSecuencias, ruta);
                hayArchivo = tablaSecuencias.getCantidad() > 0;
                tablaKmers = TablaHashKmers();  // Borramos la tabla de k-mers anterior
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

    return 0;  // 0 significa que el programa terminó bien
}
