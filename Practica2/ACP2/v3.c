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
#include <immintrin.h>
#include "counter.h"

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

    float norma = 0.0f;
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

    // Inicializar vector diagonal, donde guardamos los datos de la diagonal
    float *diagonal = (float *)aligned_alloc(alineacion, n * sizeof(float));
    for (int i = 0; i < n; i++) {
        diagonal[i] = a[i][i];
        a[i][i] = 0.0; // Al modificar la diagonal de la matriz original evitamos problemas con la condicion i!=j y la memoria alineada
    }

    // Inicializar vector temporal, usado para calcular la suma acumulada
    float *temp = (float *)aligned_alloc(alineacion, 8 * sizeof(float));

    // Método Iterativo de Jacobi
    start_counter();
    int k;
    for (k = 0; k < maxIter; k++) {
        norma = 0.0f;

        for (int i = 0; i < n; i++) {
            float suma = 0.0f;
            float *a_row = a[i];
            
            int j;
            // Desarrollamos de 8 en 8
            for (j = 0; j <= n-8 ; j += 8) {
                // Cargamos los vectores con los datos correspondientes
                __m256 vecA = _mm256_load_ps(&a_row[j]);
                __m256 vecX = _mm256_load_ps(&x[j]);
            
                // Guardamos en temp el resultado de la multiplicacion en paralelo de ambos vectores
                _mm256_store_ps(temp,  _mm256_mul_ps(vecA, vecX));
                
                // Calculamos la suma resultante
                suma = (((((((suma + temp[0]) + temp[1]) + temp[2]) +  temp[3]) + temp[4]) + temp[5]) +temp[6])  + temp[7];
                
            }

            // Suma escalar para los elementos restantes
            for (; j < n; j++){
                suma += a_row[j] * x[j];
            }

            //Calculamos el nuevo valor ayudandonos del correspondiente valor almacenado en diagonal
            float xi_new = (b[i] - suma) / diagonal[i];
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

    // Liberamos recursos
    for (int i = 0; i < n; i++) {
        free(a[i]);
    }

    free(a);
    free(b);
    free(x);
    free(xNew);
    free(diagonal);
    free(temp);

    return EXIT_SUCCESS;
}

