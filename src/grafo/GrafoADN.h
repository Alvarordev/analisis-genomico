#ifndef GRAFO_ADN_H
#define GRAFO_ADN_H

#include <string>
#include <vector>

class GrafoADN {
    public:
        GrafoADN(std::string secuencia, double beta);
        static bool esParValido(std::string par);
        double getPeso(std::string u, std::string v);
        void imprimirPares();
        void imprimirMatriz();
        int getCantidadVertices();
        int getCantidadPares();
        double getBeta();
        const std::vector<std::string>& getPares();
        const std::vector<std::string>& getVertices();
        const std::vector<std::vector<double>>& getMatriz();

    private:
        double beta;
        std::vector<std::string> pares;
        std::vector<std::string> vertices;
        std::vector<std::vector<double>> matriz;
        int indiceDe(std::string par);
        int agregarVertice(std::string par);
};

#endif
