#ifndef QAP_DATOS_H
#define QAP_DATOS_H

#include <iostream>
#include <vector>

using namespace std;

struct Datos{
    int tam;
    vector<vector<int>> flujo,
            distancia;
};

struct Solucion{
    vector<int> cambios;
    int coste,
            tam;
};

Datos leerDatos(string nombreFichero);

Solucion leerSolucion(string nombreFichero);

void compararSoluciones(const Solucion& miSolucion, const Solucion& solucionTeorica);

void mostrarResultados(const Solucion& sol);

#endif //QAP_DATOS_H
