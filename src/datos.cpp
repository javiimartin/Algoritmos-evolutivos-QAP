#include "../include/datos.h"

#include <fstream>

Datos leerDatos(string nombreFichero){
    Datos d;
    ifstream fichero(nombreFichero);

    // Abro el fichero
    if(fichero.is_open()){
        fichero >> d.tam;

        // Leo la linea en blanco
        string lineaBlanca;
        getline(fichero, lineaBlanca);

        // Leo la matriz de flujo
        d.flujo.resize(d.tam, vector<int>(d.tam));
        for(int i = 0; i < d.tam; ++i){
            for(int j = 0; j < d.tam; ++j){
                fichero >> d.flujo[i][j];
            }
        }

        //Leo la linea en blanco
        getline(fichero, lineaBlanca);

        // Leo la matriz de distancias
        d.distancia.resize(d.tam, vector<int>(d.tam));
        for(int i = 0; i < d.tam; ++i){
            for(int j = 0; j < d.tam; ++j){
                fichero >> d.distancia[i][j];
            }
        }
        // Cierro el fichero
        fichero.close();

    } else{ // Si no he podido abrir el fichero muestro un mensaje de error
        cout << "ERROR, no se ha podido abrir el fichero de datos" << endl;
    }

    return d;
}


Solucion leerSolucion(string nombreFichero){
    Solucion sol;
    ifstream fichero(nombreFichero);

    // Abro el fichero
    if(fichero.is_open()){
        fichero >> sol.tam >> sol.coste;
        // Leo el vector de cambios
        sol.cambios.resize(sol.tam);
        for(int i = 0; i < sol.tam; ++i)
            fichero >> sol.cambios[i];
        fichero.close();

    } else{ // Si no he podido abrir el fichero muestro un mensaje de error
        cout << "ERROR, no se ha podido abrir el fichero de soluciones" << endl;
    }

    return sol;
}

void compararSoluciones(const Solucion& miSolucion, const Solucion& solucionTeorica){
    cout << "******* COMPARACION DE SOLUCIONES *******" << endl << endl;
    if(miSolucion.tam != solucionTeorica.tam){
        cout << "ERROR: Las soluciones tienen tamanios diferentes" << endl;
        return;
    }

    cout << "Coste de mi solucion ---------->" << miSolucion.coste << endl;
    cout << "Coste de la solucion optima---->" << solucionTeorica.coste << endl;
    cout << "El valor fitness es ----------->" << 100.0*(miSolucion.coste - solucionTeorica.coste) / solucionTeorica.coste << endl << endl;

    cout << "Cambios en mi solucion ----------> ";
    for(int i = 0; i < miSolucion.tam; ++i)
        cout << miSolucion.cambios[i] << " ";
/*
    cout << endl << "Cambios en la solucion objetivo--> ";
    for(int i = 0; i < solucionTeorica.tam; ++i)
        cout << solucionTeorica.cambios[i] << " ";
    cout << endl;*/
}

void mostrarResultados(const Solucion& sol){
    cout << "Coste de mi solucion ---------->" << sol.coste << endl;
    //cout << "Cambios en mi solucion --------> ";
    //for(int i = 0; i < sol.tam; ++i)
    //    cout << sol.cambios[i] << " ";
    //cout << endl;
}
