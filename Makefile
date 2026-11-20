CXX      = g++
CXXFLAGS = -std=c++17 -Wall -g

EJECUTABLE = genomica

FUENTES = src/main.cpp \
          src/parser/LectorFasta.cpp \
          src/linearProbing/TablaHash.cpp \
          src/doubleHashing/TablaKmers.cpp \
          src/maxHeap/MaxHeap.cpp \
          src/grafo/GrafoADN.cpp \
          src/modoGUI/ModoGUI.cpp

OBJETOS = $(FUENTES:src/%.cpp=build/%.o)
HEADERS = $(wildcard src/*/*.h)

$(EJECUTABLE): $(OBJETOS)
	$(CXX) $(CXXFLAGS) $(OBJETOS) -o $(EJECUTABLE)

build/%.o: src/%.cpp $(HEADERS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(EJECUTABLE)
	./$(EJECUTABLE) data/HIV2_ALL_2021_env_DNA.2000.fasta

gui: $(EJECUTABLE)
	python3 gui/app.py

clean:
	rm -rf build $(EJECUTABLE)

.PHONY: run gui clean
