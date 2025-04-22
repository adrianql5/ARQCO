/*
*   Código escrito por:
*
*   Xabier Nóvoa Gómez
*
*   Adrián Quiroga Linares
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "counter.h"

// Ponemos la norma al cuadrado así evitando calcular la multiplicacion
#define tol 1e-16 
#define maxIter 20000
#define alineacion 64

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <tamaño>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    if (n <= 1) {
        printf("Introduce un tamaño válido.\n");
        exit(1);
    }

    srand(n);

    float norma = 0.0;
    double ck;

    // Reservar memoria para la matriz A
    float **a = (float **)aligned_alloc(alineacion, n * sizeof(float *));
    for (int i = 0; i < n; i++) {
        a[i] = (float *)aligned_alloc(alineacion, n * sizeof(float));
    }

    // Inicializar matriz A como diagonal dominante
    for (int i = 0; i < n; i++) {
        float sum = 0.0;
        for (int j = 0; j < n; j++) {
            a[i][j] = (float)rand() / RAND_MAX;
            sum += a[i][j];
        }
        a[i][i] += sum;
    }

    // Inicializar vector b
    float *b = (float *)aligned_alloc(alineacion, n * sizeof(float));
    for (int i = 0; i < n; i++) {
        b[i] = (float)rand() / RAND_MAX;
    }

    // Inicializar vector x
    float *x = (float *)aligned_alloc(alineacion, n * sizeof(float));
    for (int i = 0; i < n; i++) {
        x[i] = 0.0f;
    }

    // Inicializar vector xNew
    float *xNew = (float *)aligned_alloc(alineacion, n * sizeof(float));
    for (int i = 0; i < n; i++) {
        xNew[i] = 0.0f;
    }

    // Método iterativo de Jacobi
    start_counter();
    int k;
    for (k = 0; k < maxIter; k++) {
        norma = 0.0f; // Inicializamos el criterio de convergencia
        for (int i = 0; i < n; i++) {
            float *a_row = a[i]; // Guardamos el puntero a la fila en una variable
            float suma = 0.0f;

            int j;

            // Parte 1: debajo de la diagonal (j < i), desenrollado de 10 en 10
            for (j = 0; j <= i - 10; j += 10) {
                suma += a_row[j]     * x[j];
                suma += a_row[j + 1] * x[j + 1];
                suma += a_row[j + 2] * x[j + 2];
                suma += a_row[j + 3] * x[j + 3];
                suma += a_row[j + 4] * x[j + 4];
                suma += a_row[j + 5] * x[j + 5];
                suma += a_row[j + 6] * x[j + 6];
                suma += a_row[j + 7] * x[j + 7];
                suma += a_row[j + 8] * x[j + 8];
                suma += a_row[j + 9] * x[j + 9];
            }
            for (; j < i; j++) {
                suma += a_row[j] * x[j];
            }

            // Parte 2: encima de la diagonal (j > i), desenrollado de 10 en 10
            for (j = i + 1; j <= n - 10; j += 10) {
                suma += a_row[j]     * x[j];
                suma += a_row[j + 1] * x[j + 1];
                suma += a_row[j + 2] * x[j + 2];
                suma += a_row[j + 3] * x[j + 3];
                suma += a_row[j + 4] * x[j + 4];
                suma += a_row[j + 5] * x[j + 5];
                suma += a_row[j + 6] * x[j + 6];
                suma += a_row[j + 7] * x[j + 7];
                suma += a_row[j + 8] * x[j + 8];
                suma += a_row[j + 9] * x[j + 9];
            }
            for (; j < n; j++) {
                suma += a_row[j] * x[j];
            }

            float xi_new = (b[i] - suma) / a_row[i];
            float diff = xi_new - x[i]; // Almacenamos la resta en una variable evitando realizar la resta 2 veces.
            xNew[i] = xi_new;
            norma += diff * diff;
        }
        
        // Eliminamos el bucle e intercambiamos punteros
        float *temp_ptr = x;
        x = xNew;
        xNew = temp_ptr;

        // Al definir la toleracia al cuadrado evitamos el cálculo de la raíz de la norma
        if (norma < tol) break;
    }

    ck = get_counter();

    printf("Convergió en %d iteraciones con error %e\n", k, sqrtf(norma));
    printf("%1.2lf \n", ck);

    // Liberamos los recusos
    for (int i = 0; i < n; i++) {
        free(a[i]);
    }

    free(a);
    free(b);
    free(x);
    free(xNew);

    return EXIT_SUCCESS;
}




