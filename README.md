# Mini sistema de análisis genómico basado en k-mers

Proyecto de programación en C++. Lee secuencias de ADN (HIV) en formato FASTA y las
analiza usando tablas hash, un max-heap y un grafo de 2-mers.

## Cómo correr el proyecto

El proyecto se puede compilar de tres formas: **Code::Blocks** (Windows, recomendado),
los scripts **`.bat`** (Windows, sin abrir Code::Blocks) o **`make`** (Linux / macOS).
En todos los casos se genera el ejecutable `genomica.exe` (o `genomica`) en la carpeta
raíz del proyecto, que es donde lo busca la interfaz gráfica.

### Windows

#### 1. Instalar

| Programa | Para qué | Dónde |
|---|---|---|
| **Code::Blocks con MinGW** | Editor + compilador `g++` | <https://www.codeblocks.org/downloads/binaries/> → elegir el instalador que termina en **`mingw-setup.exe`** (por ejemplo `codeblocks-25.03mingw-setup.exe`). Ese incluye el compilador; el que no dice `mingw` no lo trae. |
| **Python 3** (solo para la interfaz gráfica) | Ventana Tkinter | <https://www.python.org/downloads/> → en el instalador marcar **"Add python.exe to PATH"**. Tkinter viene incluido. |

Python también se puede instalar desde una terminal (PowerShell):

```powershell
winget install -e --id Python.Python.3.12
```

Para comprobar que quedaron instalados, en una terminal nueva:

```powershell
g++ --version      # solo funciona si MinGW está en el PATH (ver paso 3, opción B)
python --version
```

#### 2. Opción A: Code::Blocks (recomendado)

1. Abrir Code::Blocks → **File → Open…** → elegir **`analisis-genomico.cbp`** (en la raíz del proyecto).
2. En la barra de herramientas, dejar el target **Release** (o **Debug**).
3. **Build → Build and run** (tecla **F9**).

Se abre una consola con el menú y el archivo `data\HIV2_ALL_2021_env_DNA.2000.fasta`
ya cargado (el proyecto lo pasa como argumento y usa la carpeta del proyecto como
directorio de trabajo).

- El proyecto ya trae configurado `-std=c++17` y `-static`. Con `-static`, `genomica.exe`
  funciona también fuera de Code::Blocks (por ejemplo desde la interfaz gráfica) sin
  pedir DLLs como `libstdc++-6.dll`.
- Si Code::Blocks dice que no encuentra el compilador: **Settings → Compiler… →
  Toolchain executables → Auto-detect**, y aceptar.

#### 3. Opción B: scripts `.bat` (sin abrir Code::Blocks)

Necesitan que `g++` esté en el `PATH`. Con el instalador de Code::Blocks, el compilador
queda en `C:\Program Files\CodeBlocks\MinGW\bin`. Para agregarlo al `PATH`:
**Inicio → "Editar las variables de entorno del sistema" → Variables de entorno… →
Path → Editar → Nuevo** → pegar esa ruta → Aceptar, y abrir una terminal nueva.

Luego, doble clic (o desde una terminal en la carpeta del proyecto):

| Script | Qué hace |
|---|---|
| `compilar.bat` | Compila y genera `genomica.exe` |
| `ejecutar.bat` | Abre el menú de texto con el archivo de HIV cargado (compila si hace falta) |
| `gui.bat` | Abre la interfaz gráfica (compila si hace falta) |

#### 4. Interfaz gráfica

Después de compilar (opción A o B), doble clic en **`gui.bat`**, o en una terminal:

```powershell
python gui\app.py
```

### Linux / macOS

```bash
# Ubuntu / Debian
sudo apt install g++ make python3-tk
# macOS (Xcode Command Line Tools trae g++/clang y make)
xcode-select --install

make            # compila → ./genomica
make run        # menú de texto con data/HIV2_ALL_2021_env_DNA.2000.fasta
make gui        # interfaz gráfica
make clean      # borra lo compilado
```

### Sin argumentos y con otros archivos

- `genomica.exe` / `./genomica` sin argumentos empieza vacío: en la opción 1, **Enter**
  carga el archivo de `data/`.
- Para otro archivo: `genomica.exe ruta\archivo.fasta` (o `./genomica ruta/archivo.fasta`).
- Las rutas relativas (como `data\...`) se resuelven desde la carpeta en la que se
  ejecuta el programa, por eso conviene ejecutarlo desde la raíz del proyecto (los
  `.bat` y Code::Blocks ya lo hacen).

### Ejemplo de uso (menú de texto)

1. Abrir el menú (F9 en Code::Blocks, `ejecutar.bat` o `make run`) → se cargan las 218
   secuencias en la tabla hash 1.
2. Opción **3** → ID `MAC.US.x.239.M33262` → muestra su secuencia.
3. Opción **4** → mismo ID y `k = 3` → construye la tabla hash 2 (64 bins).
4. Opción **6** → `ATG` → cuántas veces aparece.
5. Opción **7** → `n = 10` → los 10 3-mers más frecuentes (max-heap).
6. Opción **8** → `TCATTAGCAGTTAGCAGC` (ejemplo del enunciado) y `beta = 1` → tabla de
   pares y matriz de adyacencia. También se puede escribir el ID de una secuencia.
7. Opción **0** → salir.

### Datos

`data/HIV2_ALL_2021_env_DNA.2000.fasta`: 218 secuencias del gen *env* de HIV-2
(archivo entregado por el curso).

## Menú (parte e)

| Opción | Qué hace | Parte |
|---|---|---|
| 1 | Cargar un archivo FASTA y llenar la tabla hash 1 | a, b |
| 2 | Mostrar la tabla hash 1 (ID → secuencia) | b |
| 3 | Buscar una secuencia por su ID | b |
| 4 | Construir la tabla hash 2 (k-mer → conteo) de una secuencia | b |
| 5 | Mostrar la tabla hash 2 | b |
| 6 | Buscar cuántas veces aparece un k-mer | b |
| 7 | Ver los n k-mers más frecuentes | c |
| 8 | Grafo de 2-mers y su matriz de adyacencia | d |

## Interfaz gráfica

`gui.bat` (Windows) o `make gui` (Linux / macOS) abre una ventana con las mismas opciones del menú, agrupadas en 5 secciones:
archivo y tabla hash 1, buscar secuencia, tabla de k-mers, k-mers más frecuentes y grafo
de 2-mers (con el dibujo del grafo y la matriz de adyacencia como mapa de calor).

**Todos los cálculos siguen en C++.** La interfaz (`gui/app.py`) está hecha con Tkinter,
que viene incluido en Python, así que no hay que instalar nada. Funciona así:

```
gui/app.py  --- comandos por stdin --->  ./genomica --gui  (C++)
            <--- una línea JSON -------
```

La ventana deja abierto el proceso `./genomica --gui` y le envía comandos (`cargar`,
`kmers`, `top`, `grafo`, ...). El programa en C++ usa las mismas clases que el menú de
texto y responde en JSON. La lista de comandos está en `src/modoGUI/ModoGUI.h`. Así el estado
(las tablas hash) vive en C++ y el archivo FASTA se lee una sola vez.

## Estructura

```
src/
├── main.cpp                      (e) menú de texto
├── parser/                       (a) SecuenciaADN.h, LectorFasta.h/.cpp
├── linearProbing/                (b) tabla hash 1: TablaHash.h/.cpp
├── doubleHashing/                (b) tabla hash 2: TablaKmers.h/.cpp
├── maxHeap/                      (c) MaxHeap.h/.cpp
├── grafo/                        (d) GrafoADN.h/.cpp (Khan et al., 2024)
└── modoGUI/                      modo "--gui": comandos por stdin, respuestas JSON
gui/
└── app.py                        interfaz gráfica (Python + Tkinter, sin cálculos)
data/
└── HIV2_ALL_2021_env_DNA.2000.fasta
analisis-genomico.cbp             proyecto de Code::Blocks
compilar.bat, ejecutar.bat, gui.bat   scripts para Windows
Makefile                          compilación en Linux / macOS
```

## Decisiones de implementación

### a) Parser FASTA
- Una línea que empieza con `>` inicia un registro. El ID es el primer token después de `>`.
- La secuencia puede ocupar varias líneas. Se pasa a mayúsculas y **solo se conservan
  las letras**. Se descartan espacios, gaps de alineamiento (`-`, `.`), números y
  cualquier otro símbolo.
- Las letras ambiguas de IUPAC (`R` = A/G, `Y` = C/T, `N` = cualquiera, etc.) se
  conservan en la secuencia. La tabla de k-mers y el grafo saltan las ventanas o pares
  que las contienen. No se eliminan porque, si se borraran, las bases de los lados
  quedarían juntas y aparecerían k-mers o distancias que no existen en la secuencia real.
- Se ignoran las líneas vacías y las que empiezan con `;`.

### b) Tabla hash 1: ID → secuencia (linear probing)
- Función hash polinomial (método de Horner, a = 37) con el módulo en cada paso:
  `h = (h·37 + c) mod m`.
- Sondeo: `pos_i = (h + i) mod m`. Cada bin guarda key, valor y estado (VACIO, BORRADO, OCUPADO).
- Tamaño inicial 10. Si al insertar el factor de carga `n/m` pasaría de 1, el
  nuevo tamaño es el **siguiente primo ≥ 2n**, así el factor queda en ~0.5. Después se
  reinsertan todos los elementos (rehash).

### b) Tabla hash 2: k-mer → conteo (double hashing)
- Los k-mers se obtienen con una ventana de tamaño k que avanza de a un carácter. Las
  ventanas con letras distintas de A/C/G/T (por ejemplo `N`) se saltan.
- Tamaño `m = 4^k` (k entre 1 y 10). Hay a lo más `4^k` k-mers distintos, así que la tabla
  no se llena.
- `h1(s)` es el mismo Horner de la tabla 1 (a = 37, `mod m`). `h2(s)` usa Horner con
  b = 31 y `mod m/2`, y devuelve `2·h + 1`. Usa otra base para que los k-mers que chocan
  en `h1` no tengan también el mismo salto.
- Sondeo: `pos_i = (h1 + i·h2) mod m`. Como `m` es potencia de 2, `h2` se fuerza a
  ser **impar**. Así `mcd(h2, m) = 1` y el sondeo recorre todas las casillas.

### c) Max-heap
- Árbol binario completo guardado en un arreglo: `padre(i) = (i-1)/2`, hijos `2i+1` y `2i+2`.
- Se construye a partir de los pares de la tabla 2 con el algoritmo de Floyd (heapify) en O(n).
- Prioridad: mayor conteo primero. Si hay empate, va primero el k-mer menor alfabéticamente.
- `topN(n)` extrae n veces sobre una copia del heap.

### d) Grafo de 2-mers
- La secuencia se parte en pares **sin solapamiento** (`TCATTA…` → `TC AT TA …`). Cada
  par distinto es un vértice (como máximo 16).
- Para **cada par de posiciones i < j** hay un arco dirigido desde el par i hacia el par j,
  con peso `w = 1 / (j − i)^β`.
- Si un arco (u, v) se repite, sus pesos se **suman**. Se permiten lazos (u, u).
- Los pares con letras ambiguas (`R`, `Y`, `N`, …) no son vértices, pero su posición se
  sigue contando. Así la distancia `j − i` entre los pares válidos es la real.
- Se representa con una matriz de adyacencia V×V.
- Ejemplo del enunciado (`TCATTAGCAGTTAGCAGC`, β = 1): `w(TC,AT) = 1` y
  `w(AT,CA) = 0.1667`.

## Complejidad

| Operación | Promedio | Peor caso |
|---|---|---|
| Parser FASTA | O(L) | O(L) |
| Tabla 1: insertar / buscar | O(1) | O(n) |
| Tabla 1: rehash | O(n) | O(n²) |
| Tabla 2: insertar / buscar | O(1) | O(4^k) |
| Tabla 2: construir desde una secuencia | O(L·k) | O(L·4^k) |
| Heap: construir (Floyd) | O(n) | O(n) |
| Heap: insertar / extraer máximo | O(log n) | O(log n) |
| Heap: top-n | O(n + t·log n) | O(n + t·log n) |
| Grafo: construir | O(P²) | O(P²) |
| Grafo: memoria | O(V²) | O(V²) ≤ 16×16 |

L = largo de la secuencia, n = número de elementos, t = cantidad pedida en el top,
P = L/2 pares, V = vértices.

Notas:
- El costo del rehash incluye el sondeo de cada reinserción. Con el factor de carga
  en ~0.5 después de crecer, en promedio es O(n).
- Cada inserción en la tabla 2 compara strings de largo k, por eso construirla cuesta O(L·k).

## Referencias

- Khan RH, Salamat N, Baig AQ, Shaikh ZA, Yousef A (2024). Graph-based analysis of DNA
  sequence comparison in closed cotton species: A generalized method to unveil genetic
  connections. *PLoS ONE* 19(9): e0306608. https://doi.org/10.1371/journal.pone.0306608
- Cormen TH, Leiserson CE, Rivest RL, Stein C (2022). *Introduction to Algorithms*, 4th ed.
  MIT Press. (Capítulos sobre tablas hash con direccionamiento abierto y heaps.)
