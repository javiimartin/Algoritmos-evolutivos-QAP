#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <chrono>
#include <fstream>
#include "../include/algoritmo.h"


const double costeTai256c = 44759294;

int main() {
    Datos datos;
    datos = leerDatos("../tai256c.dat");


    // Parámetros comunes para todos los experimentos
    int tam_poblacion = 50;
    int max_evaluaciones = 50000;
    double prob_cruce = 0.7;
    double prob_mutacion = 0.1;
    vector<int> historico_mejor_coste;

    Solucion solucion;

    // Leer solución óptima para comparaciones
    Solucion solucionOptima = leerSolucion("../tai256c.sln");


    auto t1 = std::chrono::high_resolution_clock::now();

    // Ejecucion del algoritmo
    solucion = algoritmoGeneticoGeneracional(datos, tam_poblacion, prob_cruce, prob_mutacion, max_evaluaciones, "pmx", historico_mejor_coste);
    //solucion = algoritmoGeneticoBaldwin(datos, tam_poblacion, prob_cruce, prob_mutacion, max_evaluaciones, "posicion", historico_mejor_coste);
    //solucion = algoritmoGeneticoLamarck(datos, tam_poblacion, prob_cruce, prob_mutacion, max_evaluaciones, "pmx", historico_mejor_coste);
    //solucion = algoritmoGeneticoEstacionarioLamarck(datos, tam_poblacion, prob_cruce, prob_mutacion, max_evaluaciones, "pmx", historico_mejor_coste);


    auto t2 = std::chrono::high_resolution_clock::now();

     // Comparar con la solución óptima
     compararSoluciones(solucion, solucionOptima);

     // Calcular y mostrar el tiempo de ejecución
     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
     std::cout << "\n\nTiempo de ejecucion: " << duration << " ms = " << duration/1000 << "segundos = " << (duration/1000)/60 << "minutos" << endl;



    // Guardar evolucion en un fichero
    ofstream fout("../resultados/evolucionNormal.csv");
    //ofstream fout("../resultados/evolucionBaldwin.csv");
    //ofstream fout("../resultados/evolucionLamarck.csv");
    //ofstream fout("../resultados/evolucionLamarckPMX.csv");
    //ofstream fout("../resultados/evolucionEstacionarioLamarckPMX.csv");

    for(int i = 0; i < (int)historico_mejor_coste.size(); i++){
        fout << i << "," << 100.0*(historico_mejor_coste[i] - costeTai256c) / costeTai256c << "\n";
    }
    fout.close();


    return 0;
}