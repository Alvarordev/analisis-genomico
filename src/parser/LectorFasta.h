#ifndef LECTOR_FASTA_H
#define LECTOR_FASTA_H

#include "SecuenciaADN.h"
#include <string>
#include <vector>

class LectorFasta {
    public:
        static std::vector<SecuenciaADN> leerArchivo(std::string ruta);

    private:
        static std::string limpiarLinea(std::string linea);
};

#endif
