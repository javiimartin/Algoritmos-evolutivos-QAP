#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <chrono>
#include <set>
#include <map>
#include  <cmath>
#include <climits>
#include "../include/algoritmo.h"

using namespace std;


bool comparaSoluciones(const Solucion &a, const Solucion &b) {
    return a.coste < b.coste;
}

// Calculo el coste de una solucion respecto a unos datos
int calcularCoste(const Solucion& vecino, const Datos& datos){
    int coste = 0;
    for(int i = 0; i < datos.tam; ++i)
        for(int j = 0; j < datos.tam; ++j)
            coste += datos.flujo[i][j] * datos.distancia[vecino.cambios[i]][vecino.cambios[j]];

    return coste;
}


Solucion generarSolucionInicial(const Datos& d){
    Solucion sol;
    sol.tam = d.tam;
    sol.cambios.resize(d.tam);
    iota(sol.cambios.begin(), sol.cambios.end(), 0);
    shuffle(sol.cambios.begin(), sol.cambios.end(), default_random_engine());
    sol.coste = calcularCoste(sol, d);

    return sol;
}



// Función para calcular el cambio en el coste al intercambiar dos ubicaciones
int calcularDelta(const Datos& datos, const Solucion& solActual, int r, int s) {
    int delta = 0;
    for (int k = 0; k < datos.tam; ++k) {
        if (k != r && k != s) {
            delta += datos.flujo[r][k] * (datos.distancia[solActual.cambios[s]][solActual.cambios[k]] - datos.distancia[solActual.cambios[r]][solActual.cambios[k]]) +
                     datos.flujo[k][r] * (datos.distancia[solActual.cambios[k]][solActual.cambios[s]] - datos.distancia[solActual.cambios[k]][solActual.cambios[r]]) +
                     datos.flujo[s][k] * (datos.distancia[solActual.cambios[r]][solActual.cambios[k]] - datos.distancia[solActual.cambios[s]][solActual.cambios[k]]) +
                     datos.flujo[k][s] * (datos.distancia[solActual.cambios[k]][solActual.cambios[r]] - datos.distancia[solActual.cambios[k]][solActual.cambios[s]]);
        }
    }
    return delta;
}

Solucion busquedaLocal(const Datos& datos, int max_evaluaciones, Solucion& individuoInicial) {
    // Copiamos la solución inicial
    Solucion solActual = individuoInicial;
    // Aseguramos su coste
    solActual.coste = calcularCoste(solActual, datos);

    int evaluaciones = 0;
    bool hayMejora = true;

    while (hayMejora && (evaluaciones < max_evaluaciones)) {
        hayMejora = false;

        for (int r = 0; r < datos.tam && !hayMejora; r++) {
            for (int s = r + 1; s < datos.tam && !hayMejora; s++) {
                // Calculamos el delta de intercambiar r y s
                int delta = calcularDelta(datos, solActual, r, s);
                evaluaciones++;

                // Si mejora (delta < 0), aplicamos inmediatamente
                if (delta < 0) {
                    // Intercambiamos
                    swap(solActual.cambios[r], solActual.cambios[s]);
                    // Ajustamos el coste
                    solActual.coste += delta;
                    hayMejora = true;
                }

                // Si ya alcanzamos el límite de evaluaciones, salimos
                if (evaluaciones >= max_evaluaciones)
                    break;
            }
        }
    }

    return solActual;
}




vector<Solucion> seleccionTorneo(const vector<Solucion> &P){
    int tam_poblacion = P.size();
    vector<Solucion> P_seleccionada;

    for (int i = 0; i < tam_poblacion; i++) {
        int indice1 = rand() % tam_poblacion;
        int indice2 = rand() % tam_poblacion;

        const Solucion& individuo1 = P[indice1];
        const Solucion& individuo2 = P[indice2];

        double probabilidad = static_cast<double>(rand()) / RAND_MAX;
        Solucion seleccionado;

        if(P[indice1].coste <= P[indice2].coste)
            P_seleccionada.push_back(P[indice1]);
        else
            P_seleccionada.push_back(P[indice2]);

    }

    return P_seleccionada;
}


pair<Solucion, Solucion> crucePosicion(const Solucion& padre1, const Solucion& padre2) {
    int tam = padre1.tam;
    Solucion hijo1, hijo2;
    hijo1.tam = hijo2.tam = tam;
    hijo1.cambios.assign(tam,-1);
    hijo2.cambios.assign(tam,-1);
    set<int> diferentes;

    for(int i = 0; i < tam; ++i){
        if(padre1.cambios[i] == padre2.cambios[i]){
            hijo1.cambios[i] = padre1.cambios[i];
            hijo2.cambios[i] = padre1.cambios[i];
        }
        else{
            diferentes.insert(padre1.cambios[i]);
            diferentes.insert(padre2.cambios[i]);
        }
    }
    vector<int> introducir(diferentes.begin(), diferentes.end());

    shuffle(introducir.begin(), introducir.end(), default_random_engine());
    for(int i = 0, j = 0; i < tam; ++i)
        if(hijo1.cambios[i] == -1)
            hijo1.cambios[i] = introducir[j++];

    shuffle(introducir.begin(), introducir.end(), default_random_engine());
    for(int i = 0, j = 0; i < tam; ++i)
        if(hijo2.cambios[i] == -1)
            hijo2.cambios[i] = introducir[j++];


    return {hijo1, hijo2};

}


pair<Solucion, Solucion> crucePMX(const Solucion& padre1, const Solucion& padre2) {
    int tam = padre1.tam;
    Solucion hijo1, hijo2;
    hijo1.tam = hijo2.tam = tam;
    hijo1.cambios.assign(tam, -1);
    hijo2.cambios.assign(tam, -1);

    // Seleccionar un rango aleatorio en los padres
    default_random_engine generator;
    uniform_int_distribution<int> distribution(0, tam - 1);
    int inicio = distribution(generator);
    int fin = distribution(generator);
    if (inicio > fin) {
        swap(inicio, fin);
    }

    // Crear mapas de correspondencias
    map<int, int> mapa1, mapa2;

    // Copiar el rango en los hijos y llenar los mapas
    for (int i = inicio; i <= fin; ++i) {
        hijo1.cambios[i] = padre2.cambios[i];
        hijo2.cambios[i] = padre1.cambios[i];
        mapa1[padre2.cambios[i]] = padre1.cambios[i];
        mapa2[padre1.cambios[i]] = padre2.cambios[i];
    }

    // Completar los hijos
    for (int i = 0; i < tam; ++i) {
        if (i >= inicio && i <= fin) continue;
        int valor1 = padre1.cambios[i];
        int valor2 = padre2.cambios[i];
        while (mapa1.find(valor1) != mapa1.end()) {
            valor1 = mapa1[valor1];
        }
        while (mapa2.find(valor2) != mapa2.end()) {
            valor2 = mapa2[valor2];
        }
        hijo1.cambios[i] = valor1;
        hijo2.cambios[i] = valor2;
    }

    return {hijo1, hijo2};
}



void mutar(vector<Solucion>& P, const Datos& datos, double tasa_mutacion) {
    int tam_poblacion = P.size();
    int tam_gen = P[0].tam;

    for (int i = 0; i < tam_poblacion*tasa_mutacion; i++) {
        int indice_muta = rand() % tam_poblacion;
        int indice_gen = rand() % tam_gen;
        int indice_otro_gen = rand() % tam_gen;

        // Verificar que los índices de los genes sean diferentes
        while (indice_gen == indice_otro_gen) {
            indice_otro_gen = rand() % tam_gen;
        }

        // Intercambiar los genes seleccionados
        swap(P[indice_muta].cambios[indice_gen], P[indice_muta].cambios[indice_otro_gen]);
    }

}


vector<Solucion> generarPoblacionInicial(const Datos& datos, int tam_poblacion) {
    vector<Solucion> P;
    for (int i = 0; i < tam_poblacion; i++) {
        Solucion individuo = generarSolucionInicial(datos);
        P.push_back(individuo);
    }
    return P;
}

Solucion algoritmoGeneticoGeneracional(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion, int criterio_parada, string cruce, vector<int>& historico_mejor_coste) {
    // Generar población inicial
    vector<Solucion> P_actual = generarPoblacionInicial(datos, tam_poblacion);

    // Evaluar población inicial
    for (Solucion& individuo : P_actual)
        individuo.coste = calcularCoste(individuo, datos);


    // Bucle principal
    int evaluaciones = tam_poblacion;

    while (evaluaciones < criterio_parada) {
        // Selección
        vector<Solucion> P_seleccionada = seleccionTorneo(P_actual);

        // Cruce
        vector<Solucion> P_hijos;
        for (int i = 0; i < tam_poblacion*probabilidad_cruce; ++i) {
            int indice_p1 = rand() % tam_poblacion;
            int indice_p2 = rand() % tam_poblacion;
            const Solucion& padre1 = P_seleccionada[indice_p1];
            const Solucion& padre2 = P_seleccionada[indice_p2];
            pair<Solucion, Solucion> hijos;

            if(cruce == "posicion")
                hijos = crucePosicion(padre1, padre2);
            else
                hijos = crucePMX(padre1, padre2);

            P_hijos.push_back(hijos.first);
            P_hijos.push_back(hijos.second);
        }

        // Mutación
        mutar(P_hijos, datos, probabilidad_mutacion);

        // Evaluación de la población de hijos
        for (Solucion& individuo : P_hijos)
            individuo.coste = calcularCoste(individuo, datos);


        // Reemplazo
        P_actual.insert(P_actual.end(), P_hijos.begin(), P_hijos.end());
        sort(P_actual.begin(), P_actual.end(), comparaSoluciones);
        P_actual.resize(tam_poblacion);

        // Actualizar el número de evaluaciones
        evaluaciones += tam_poblacion;

        // Guardar el mejor coste
        if(evaluaciones % 500 == 0)
            historico_mejor_coste.push_back(P_actual[0].coste);
    }

    // Encontrar el mejor individuo en la última población
    sort(P_actual.begin(), P_actual.end(), comparaSoluciones);

    historico_mejor_coste.push_back(P_actual[0].coste);

    return P_actual[0];
}



Solucion algoritmoGeneticoBaldwin(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion,
                                  int max_evaluaciones, string cruce, vector<int>& historico_mejor_coste) {
    // Generar población inicial
    vector<Solucion> P_actual = generarPoblacionInicial(datos, tam_poblacion);

    // Evaluar población inicial con aprendizaje local (Baldwin)
    for (Solucion& individuo : P_actual) {
        // Generamos una copia para "aprender" localmente
        Solucion individuoAprendido = individuo;
        // Aplicamos búsqueda local para mejorar la solución
        individuoAprendido = busquedaLocal(datos,  5000, individuoAprendido);

        // Asignamos el coste del individuo "aprendido" como fitness
        individuo.coste = individuoAprendido.coste;
    }

    // Contamos evaluaciones (hemos evaluado la población inicial)
    int evaluaciones = tam_poblacion;

    // Bucle principal
    while (evaluaciones < max_evaluaciones) {

        // Selección
        vector<Solucion> P_seleccionada = seleccionTorneo(P_actual);

        // Cruce
        vector<Solucion> P_hijos;
        for (int i = 0; i < tam_poblacion*probabilidad_cruce; ++i) {
            int indice_p1 = rand() % tam_poblacion;
            int indice_p2 = rand() % tam_poblacion;
            const Solucion& padre1 = P_seleccionada[indice_p1];
            const Solucion& padre2 = P_seleccionada[indice_p2];
            pair<Solucion, Solucion> hijos;

            if(cruce == "posicion")
                hijos = crucePosicion(padre1, padre2);
            else
                hijos = crucePMX(padre1, padre2);

            P_hijos.push_back(hijos.first);
            P_hijos.push_back(hijos.second);
        }

        // Mutación
        mutar(P_hijos, datos, probabilidad_mutacion);

        // Evaluación de los hijos
        for (Solucion& hijo : P_hijos) {
            Solucion hijoAprendido = hijo;
            hijoAprendido = busquedaLocal(datos, 5000, hijoAprendido); // Aplicamos busqueda local
            hijo.coste = hijoAprendido.coste;
        }

        // Reemplazo
        P_actual.insert(P_actual.end(), P_hijos.begin(), P_hijos.end());
        sort(P_actual.begin(), P_actual.end(), comparaSoluciones);
        P_actual.resize(tam_poblacion);

        // Actualizar número de evaluaciones
        evaluaciones += tam_poblacion;

        // Guardar el mejor coste
        if(evaluaciones % 500 == 0)
            historico_mejor_coste.push_back(P_actual[0].coste);
    }

    // Devolver el mejor individuo tras terminar
    sort(P_actual.begin(), P_actual.end(), comparaSoluciones);

    historico_mejor_coste.push_back(P_actual[0].coste);

    return P_actual[0];
}


Solucion algoritmoGeneticoLamarck(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion,
                                  int max_evaluaciones, string cruce, vector<int>& historico_mejor_coste) {
    // Generar población inicial
    vector<Solucion> P_actual = generarPoblacionInicial(datos, tam_poblacion);

    // Evaluar población inicial CON APRENDIZAJE LAMARCKIANO
    for (Solucion &individuo : P_actual) {
        // Saca una copia para búsqueda local
        Solucion individuoMejorado = individuo;
        individuoMejorado = busquedaLocal(datos, /*max_evals*/ 5000, individuoMejorado);

        // Actualizamos coste y cambios obtenidos
        individuo.cambios = individuoMejorado.cambios;
        individuo.coste   = individuoMejorado.coste;
    }


    int evaluaciones = tam_poblacion;

    // Bucle principal
    while (evaluaciones < max_evaluaciones) {

        // Selección
        vector<Solucion> P_seleccionada = seleccionTorneo(P_actual);

        // Cruce
        vector<Solucion> P_hijos;
        for (int i = 0; i < tam_poblacion * probabilidad_cruce; ++i) {
            int indice_p1 = rand() % tam_poblacion;
            int indice_p2 = rand() % tam_poblacion;
            const Solucion &padre1 = P_seleccionada[indice_p1];
            const Solucion &padre2 = P_seleccionada[indice_p2];

            pair<Solucion, Solucion> hijos;
            if (cruce == "posicion")
                hijos = crucePosicion(padre1, padre2);
            else
                hijos = crucePMX(padre1, padre2);

            P_hijos.push_back(hijos.first);
            P_hijos.push_back(hijos.second);
        }

        // Mutación
        mutar(P_hijos, datos, probabilidad_mutacion);

        // Evaluación con busqueda local
        for (Solucion &hijo : P_hijos) {
            Solucion hijoMejorado = hijo;
            hijoMejorado = busquedaLocal(datos, 5000, hijoMejorado);

            hijo.cambios = hijoMejorado.cambios;
            hijo.coste   = hijoMejorado.coste;
        }

        // Reemplazo
        P_actual.insert(P_actual.end(), P_hijos.begin(), P_hijos.end());
        sort(P_actual.begin(), P_actual.end(), comparaSoluciones);
        P_actual.resize(tam_poblacion);

        // Actualizar contador de evaluaciones
        evaluaciones += tam_poblacion;

        // Guardar el mejor coste
        if(evaluaciones % 500 == 0)
            historico_mejor_coste.push_back(P_actual[0].coste);
    }

    // Devolver el mejor individuo
    sort(P_actual.begin(), P_actual.end(), comparaSoluciones);

    historico_mejor_coste.push_back(P_actual[0].coste);

    return P_actual[0];
}


Solucion algoritmoGeneticoEstacionario(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion,
                                       int criterio_parada, string cruce, vector<int>& historico_mejor_coste) {
    // Generar población inicial
    vector<Solucion> P_actual = generarPoblacionInicial(datos, tam_poblacion);

    // Evaluar población inicial
    for (Solucion& individuo : P_actual)
        individuo.coste = calcularCoste(individuo, datos);

    // Bucle principal
    int evaluaciones = tam_poblacion;

    while (evaluaciones < criterio_parada) {
        // Selección
        vector<Solucion> P_seleccionada = seleccionTorneo(P_actual);

        // Cruce
        vector<Solucion> P_hijos;
        for (int i = 0; i < 2; ++i) { // Solo dos descendientes
            int indice_p1 = rand() % tam_poblacion;
            int indice_p2 = rand() % tam_poblacion;
            const Solucion& padre1 = P_seleccionada[indice_p1];
            const Solucion& padre2 = P_seleccionada[indice_p2];
            pair<Solucion, Solucion> hijos;

            if(cruce == "posicion")
                hijos = crucePosicion(padre1, padre2);
            else
                hijos = crucePMX(padre1, padre2);

            // Agregar hijos a la población temporal
            P_hijos.push_back(hijos.first);
            P_hijos.push_back(hijos.second);
        }

        // Mutación
        mutar(P_hijos, datos, probabilidad_mutacion);

        // Evaluación de los hijos
        for (Solucion& individuo : P_hijos)
            individuo.coste = calcularCoste(individuo, datos);

        // Insertar hijos en la población
        P_actual.insert(P_actual.end(), P_hijos.begin(), P_hijos.end());

        // Reemplazo: mantener las tam_poblacion mejores soluciones
        sort(P_actual.begin(), P_actual.end(), comparaSoluciones);
        P_actual.resize(tam_poblacion);

        // Actualizar el número de evaluaciones
        evaluaciones += 2; // Dos hijos evaluados por generación
    }

    // Encontrar el mejor individuo en la última población
    sort(P_actual.begin(), P_actual.end(), comparaSoluciones);

    return P_actual[0];
}

Solucion algoritmoGeneticoEstacionarioLamarck(const Datos& datos, int tam_poblacion, double probabilidad_cruce, double probabilidad_mutacion,
                                       int criterio_parada, string cruce, vector<int>& historico_mejor_coste) {
    // Generar población inicial
    vector<Solucion> P_actual = generarPoblacionInicial(datos, tam_poblacion);

    // Evaluar población inicial CON APRENDIZAJE LAMARCKIANO
    for (Solucion &individuo : P_actual) {
        // Saca una copia para búsqueda local
        Solucion individuoMejorado = individuo;
        individuoMejorado = busquedaLocal(datos, 5000, individuoMejorado);

        // *** Dif. con Baldwin: aquí SÍ actualizamos el genotipo original ***
        individuo.cambios = individuoMejorado.cambios;
        individuo.coste   = individuoMejorado.coste;
    }

    // Bucle principal
    int evaluaciones = tam_poblacion;

    while (evaluaciones < criterio_parada) {
        if (evaluaciones % 500 == 0)
            cout << "Evaluaciones: " << evaluaciones << endl;

        // Selección
        vector<Solucion> P_seleccionada = seleccionTorneo(P_actual);

        // Cruce
        vector<Solucion> P_hijos;
        for (int i = 0; i < 2; ++i) { // Solo dos descendientes
            int indice_p1 = rand() % tam_poblacion;
            int indice_p2 = rand() % tam_poblacion;
            const Solucion& padre1 = P_seleccionada[indice_p1];
            const Solucion& padre2 = P_seleccionada[indice_p2];
            pair<Solucion, Solucion> hijos;

            if(cruce == "posicion")
                hijos = crucePosicion(padre1, padre2);
            else
                hijos = crucePMX(padre1, padre2);

            // Agregar hijos a la población temporal
            P_hijos.push_back(hijos.first);
            P_hijos.push_back(hijos.second);
        }

        // Mutación
        mutar(P_hijos, datos, probabilidad_mutacion);

        // Evaluación con busqueda local
        for (Solucion &hijo : P_hijos) {
            Solucion hijoMejorado = hijo;
            hijoMejorado = busquedaLocal(datos, 5000, hijoMejorado);

            hijo.cambios = hijoMejorado.cambios;
            hijo.coste   = hijoMejorado.coste;
        }

        // Insertar hijos en la población
        P_actual.insert(P_actual.end(), P_hijos.begin(), P_hijos.end());

        // Reemplazo: mantener las tam_poblacion mejores soluciones
        sort(P_actual.begin(), P_actual.end(), comparaSoluciones);
        P_actual.resize(tam_poblacion);

        // Actualizar el número de evaluaciones
        evaluaciones += 2; // Dos hijos evaluados por generación

        // Guardar el mejor coste
        if(evaluaciones % 500 == 0)
            historico_mejor_coste.push_back(P_actual[0].coste);
    }

    // Encontrar el mejor individuo en la última población
    sort(P_actual.begin(), P_actual.end(), comparaSoluciones);

    historico_mejor_coste.push_back(P_actual[0].coste);

    return P_actual[0];
}



