#ifndef TABLA_KMERS_H
#define TABLA_KMERS_H

#include <string>
#include <vector>

struct KmerConteo {
    std::string kmer;
    int conteo = 0;
};

class TablaKmers {
    public:
        TablaKmers(int k);
        static int hash1(std::string kmer, int length);
        static int hash2(std::string kmer, int length);
        bool insertar(std::string kmer);
        int buscar(std::string kmer);
        void construirDesde(std::string secuencia);
        std::vector<KmerConteo> obtenerKmers();
        void imprimir();
        int getK();
        int getLength();
        int getDistintos();
        int getTotales();
        int getColisiones();
        double getFactorCarga();
        const std::vector<KmerConteo>& getBins();

    private:
        int k;
        std::vector<KmerConteo> bins;
        int distintos;
        int totales;
        int colisiones;
};

#endif
