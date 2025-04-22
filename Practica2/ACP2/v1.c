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

#define tol 1e-8
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
        norma = 0.0; // Inicializamos el criterio de convergencia
        for (int i = 0; i < n; i++) {
            float suma = 0.0;
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    suma += a[i][j] * x[j];
                }
            }
            xNew[i] = (b[i] - suma) / a[i][i]; // Empleamos la fórmula de Jacobi
            norma += (xNew[i] - x[i]) * (xNew[i] - x[i]); // Calculamos la norma
        }
        
        // Actualizamos el vector de solución
        for (int i = 0; i < n; i++) {
            x[i] = xNew[i];
        }

        if (sqrtf(norma) < tol) {
            break;
        }
    }
    ck = get_counter();

    printf("Convergió en %d iteraciones con error %e\n", k, sqrtf(norma));
    printf("%1.2lf \n", ck);

    //Liberamos los recursos
    for (int i = 0; i < n; i++) {
        free(a[i]);
    }

    free(a);
    free(b);
    free(x);
    free(xNew);

    return EXIT_SUCCESS;
}

