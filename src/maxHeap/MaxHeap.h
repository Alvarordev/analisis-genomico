#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "../doubleHashing/TablaKmers.h"
#include <vector>

class MaxHeap {
    public:
        MaxHeap();
        MaxHeap(std::vector<KmerConteo> elementos);
        void insertar(KmerConteo elemento);
        KmerConteo extraerMaximo();
        KmerConteo verMaximo();
        std::vector<KmerConteo> topN(int n);
        int getLength();
        bool estaVacio();

    private:
        std::vector<KmerConteo> datos;
        static int padre(int i);
        static int hijoIzquierdo(int i);
        static int hijoDerecho(int i);
        static bool tieneMasPrioridad(KmerConteo a, KmerConteo b);
        void subir(int i);
        void bajar(int i);
};

#endif
