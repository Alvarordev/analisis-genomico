# =============================================================================
# Makefile: compila el proyecto con un solo comando.
#
#   make        -> compila y genera el ejecutable "genomica"
#   make run    -> compila y ejecuta con el archivo de prueba
#   make clean  -> borra los archivos compilados
#
# Cómo funciona: cada .cpp se compila por separado a un archivo objeto (.o)
# y al final todos los .o se "enlazan" en un solo ejecutable. Si cambias un
# solo .cpp, make vuelve a compilar solo ese archivo.
# =============================================================================

# Compilador y opciones:
#   -std=c++17  usa el estándar C++17
#   -Wall       muestra advertencias (warnings) útiles
#   -g          agrega información para depurar
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -g

# Nombre del ejecutable
EJECUTABLE = genomica

# Archivos objeto: uno por cada .cpp de la carpeta src
OBJETOS = build/main.o \
          build/LectorFasta.o \
          build/TablaHashSecuencias.o \
          build/TablaHashKmers.o \
          build/MaxHeap.o \
          build/GrafoADN.o

# Regla principal: para crear el ejecutable se necesitan todos los .o
$(EJECUTABLE): $(OBJETOS)
	$(CXX) $(CXXFLAGS) $(OBJETOS) -o $(EJECUTABLE)

# Regla para cada .o: se crea a partir de su .cpp (y se recompila si cambia
# cualquier .h). $< es el .cpp y $@ es el .o que se va a generar.
build/%.o: src/%.cpp src/*.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(EJECUTABLE)
	./$(EJECUTABLE) data/muestra.fasta

clean:
	rm -rf build $(EJECUTABLE)

# Indica que "run" y "clean" no son nombres de archivos.
.PHONY: run clean
