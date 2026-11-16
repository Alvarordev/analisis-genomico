# Mini sistema de análisis genómico basado en k-mers

Proyecto de programación en C++. Lee secuencias de ADN (HIV) en formato FASTA y las
analiza usando tablas hash, un max-heap y un grafo de 2-mers.

## Compilar y ejecutar

Requiere `g++` (C++17) y `make`.

```bash
make                            # compila y genera el ejecutable ./genomica
make run                        # ejecuta con el archivo de prueba data/muestra.fasta
./genomica ruta/archivo.fasta   # ejecuta con otro archivo FASTA
make clean                      # borra lo compilado
```

> `data/muestra.fasta` tiene **datos de prueba sintéticos** (los dos ejemplos del
> enunciado y 10 secuencias aleatorias). Para el análisis real se debe usar el
> archivo FASTA de HIV del curso.

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

## Estructura

```
src/
├── SecuenciaADN.h            struct con id y secuencia
├── LectorFasta.h/.cpp        (a) parser FASTA
├── TablaHashSecuencias.h/.cpp (b) tabla hash 1: linear probing
├── TablaHashKmers.h/.cpp     (b) tabla hash 2: double hashing
├── MaxHeap.h/.cpp            (c) max-heap para los k-mers más frecuentes
├── GrafoADN.h/.cpp           (d) grafo de 2-mers (Khan et al., 2024)
└── main.cpp                  (e) menú de texto
```

## Decisiones de implementación

### a) Parser FASTA
- Una línea que empieza con `>` inicia un registro. El ID es el primer token después de `>`.
- La secuencia puede ocupar varias líneas. Se pasa a mayúsculas y se le quitan los
  espacios y los gaps de alineamiento (`-`, `.`).
- Se ignoran las líneas vacías y las que empiezan con `;`.

### b) Tabla hash 1: ID → secuencia (linear probing)
- Función hash polinomial: `h(s) = Σ s[i]·31^(m-1-i) mod 2^64`, luego `mod tamaño`.
- Sondeo: `pos_i = (h + i) mod m`.
- Tamaño inicial 10. Si al insertar el factor de carga `n/m` pasaría de 1, el
  nuevo tamaño es el **siguiente primo ≥ 2n**, así el factor queda en ~0.5. Después se
  reinsertan todos los elementos (rehash).

### b) Tabla hash 2: k-mer → conteo (double hashing)
- Los k-mers se obtienen con una ventana de tamaño k que avanza de a un carácter. Las
  ventanas con letras distintas de A/C/G/T (por ejemplo `N`) se saltan.
- Tamaño `m = 4^k` (k entre 1 y 10). Hay a lo más `4^k` k-mers distintos, así que la tabla
  no se llena.
- `h1(s)` es el hash polinomial en base 31 y `h2(s) = 2·(hash_base37(s) mod m/2) + 1`.
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
