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
// Librería para procesamieto paralelo a nivel de memoria
#include <omp.h> 

#include "counter.h"

#define tol 1e-16
#define maxIter 20000
#define alineacion 64

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Uso: %s <tamaño> <numHilos>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    int numHilos = atoi(argv[2]);

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
    float *xNew = (float *)aligned_alloc(alineacion, n * sizeof(float));
    for (int i = 0; i < n; i++) {
        x[i] = 0.0f;
        xNew[i] = 0.0f;
    }

    start_counter();
    int k;
    for (k = 0; k < maxIter; k++) {
        norma = 0.0f;
        // Sección de código paralea con numHilos, reparto de iteraciones static
        // La variable norma es sumada por cada hilo de forma independiente y después se suma el resultado de cada hilo
        #pragma omp parallel for num_threads(numHilos) reduction(+:norma) schedule(static)
        for (int i = 0; i < n; i++) {
            float *a_row = a[i];
            float suma = 0.0f;
            int j;

            // Parte 1: debajo de la diagonal
            for (j = 0; j <= i - 10; j += 10) {
                suma += a_row[j] * x[j];
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

            // Parte 2: encima de la diagonal
            for (j = i + 1; j <= n - 10; j += 10) {
                suma += a_row[j] * x[j];
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
            float diff = xi_new - x[i];
            xNew[i] = xi_new;
            norma += diff * diff;
        }
        
        float *temp_ptr = x;
        x = xNew;
        xNew = temp_ptr;

        if (norma < tol) break;
    }

    ck = get_counter();

    printf("Convergió en %d iteraciones con error %e\n", k, sqrtf(norma));
    printf("%1.2lf \n", ck);

    // Liberamos recusos
    for (int i = 0; i < n; i++) {
        free(a[i]);
    }

    free(a);
    free(b);
    free(x);
    free(xNew);

    return EXIT_SUCCESS;
}

