#ifndef TABLA_HASH_H
#define TABLA_HASH_H

#include <string>
#include <vector>


enum Estado {
    VACIO,
    BORRADO,
    OCUPADO
};

struct Bin {
    std::string key;
    std::string valor;
    Estado estado = VACIO;
};

class TablaHash {
    public:
        TablaHash(int tamano);
        static int funcionHash(std::string key, int length);
        bool insertar(std::string key, std::string valor);
        std::string* buscar(std::string key);
        void imprimir();
        int getNumKeys();
        int getLength();
        double getFactorCarga();
        int getColisiones();
        int getRedimensionamientos();
        const std::vector<Bin>& getBins();

    private:
        std::vector<Bin> bins;
        int num_keys;
        int colisiones;
        bool insertarSinRedimensionar(std::string key, std::string valor);
        void redimensionar(int nuevoTamano);
        static bool esPrimo(int n);
        static int siguientePrimo(int n);
        int redimensionamientos;
};

#endif
