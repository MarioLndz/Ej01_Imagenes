#include <iostream>
#include <chrono>
#include <vector>
#include <stdlib.h>
#include <image.h>

using namespace std;


void chrono_experiment(int rows, int cols, int repetitions) {
    // Inicializamos el experimento antes del bucle de conteo de tiempo para no afectar al análisis
    Image image(rows, cols, 0);
    /*for (int i = 0; i < rows; ++i){
        for (int j = 0; j < cols; ++j){

            image.set_pixel(i, j, byte(rand() % 1000));

        }

    }*/

    // Medimos el tiempo antes de iniciar el expermiento
    chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();

    // Realizamos el experimento tantas veces como indique repetitions, para tener cierta robustez
    // en el resultado (no todas las ejecuciones tardan lo mismo)
    for (int k = 0; k < repetitions; ++k){
        image.ShuffleRows();
    }

    // Medimos el tiempo al finalizar el experimento
    chrono::high_resolution_clock::time_point finish_time = chrono::high_resolution_clock::now();

    // Calculamos la diferencia entre el inicio y el final
    chrono::duration<double> total_duration = chrono::duration_cast<chrono::duration<double>>(finish_time - start_time);

    // Dividimos el tiempo total entre el número de repeticiones para obtener el tiempo medio por ejecución
    cout << rows << "\t" << total_duration.count()/repetitions << endl;
}
int main() {
    for (int i = 100; i <= 2500; i += 100){
        chrono_experiment(i,i, 30);
    }
    return 0;
}