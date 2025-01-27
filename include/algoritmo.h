#ifndef QAP_ALGORITMO_H
#define QAP_ALGORITMO_H

#include <iostream>
#include <vector>
#include "datos.h"

using namespace std;

// Función para calcular el coste de una solución
int calcularCoste(const Solucion& vecino, const Datos& datos);


// Función para generar una solución inicial
Solucion generarSolucionInicial(const Datos& d);


Solucion busquedaLocal(const Datos& datos, int max_evaluaciones, Solucion& individuoInicial);


//Función para la mutación
void mutar(vector<Solucion>& P, const Datos& datos, double tasa_mutacion);

// Función para el cruce basada en posición
pair<Solucion, Solucion> crucePosicion(const Solucion &padre1, const Solucion &padre2);

// Función para el cruce PMX
pair<Solucion, Solucion> crucePMX(const Solucion &padre1, const Solucion &padre2);

// Función de selección de torneo
vector<Solucion> seleccionTorneo(const vector<Solucion> &P);


// Función principal del AG
Solucion algoritmoGeneticoGeneracional(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion, int evaluaciones, string cruce, vector<int>& historico_mejor_coste);

Solucion algoritmoGeneticoEstacionario(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion, int evaluaciones, string cruce, vector<int>& historico_mejor_coste);

Solucion algoritmoGeneticoBaldwin(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion, int max_evaluaciones, string cruce, vector<int>& historico_mejor_coste);

Solucion algoritmoGeneticoLamarck(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion, int max_evaluaciones, string cruce, vector<int>& historico_mejor_coste);

Solucion algoritmoGeneticoEstacionarioLamarck(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion, int criterio_parada, string cruce, vector<int>& historico_mejor_coste);

#endif //QAP_ALGORITMO_H
