#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void start_counter();
double get_counter();

static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;

/* Set *hi and *lo to the high and low order bits of the cycle counter */
void access_counter(unsigned *hi, unsigned *lo) {
    asm volatile("rdtsc; movl %%edx, %0; movl %%eax, %1"
                 : "=r"(*hi), "=r"(*lo)
                 :
                 : "%edx", "%eax");
}

/* Record the current value of the cycle counter */
void start_counter() {
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter */
double get_counter() {
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    access_counter(&ncyc_hi, &ncyc_lo);

    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double)hi * (1UL << 30) * 4 + lo;

    if (result < 0) {
        fprintf(stderr, "Error: counter returns negative value: %.0f\n", result);
    }
    return result;
}


/* Funcion para crear un vector random */
void crearVectorRand(int *arr, int size,int D) {
    srand(time(NULL));
    for (int i = 0; i < size; i+=D) {
        int valor = ((int)rand());
	    int signo = (rand()%2==0) ?1: -1;
	    arr[i] = signo*valor;
    }
}

int main(int argc, char **argv) {

    // Comprobar que se han introducido los argumentos
    if (argc < 4) {
        printf("Uso: %s <D> <R>\n", argv[0]);
        return 1;
    }

    int D = atoi(argv[1]);
    int R = atoi(argv[2]);
    int L = atoi(argv[3]);

    /* Asignación de memoria alineada */

    
    int *A = (int *)aligned_alloc(64, R * D * sizeof(int));
    if (A == NULL) {
        printf("Error al asignar memoria.\n");
        return 1;
    }

    crearVectorRand(A, R*D,D);

    int *ind = (int *)malloc(R * sizeof(int));
    if (ind == NULL) {
        printf("Error al asignar memoria.\n");
        free(A);
        return 1;
    }

    for (int i = 0; i < R; i++) {
        ind[i] = D * i;
    }

    int suma = 0;
    int  S[10] = {0}; // Vector S
    double ck;

    /* Medición del contador */
    start_counter();

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < R; j++) {
            int index = ind[j];
            suma += A[index];
        }
        S[i] = suma;
        suma = 0.0;
    }

    ck = get_counter();
    

    double s;
    for(int i = 0; i<10; i++){ // Su
	s+=S[i];
    }
    s=s/10;
    //printf("%lf",s); Valor medio de s
    //printf("%lf\n", ck/10); ciclos de reloj por bucle
    //printf("%lf\n", ck/(10*R)); ciclos de reloj por acceso

    printf("%d,%d,%d,%.3lf,%.3lf,%.3lf\n", D, R, L, s, ck/10.0, ck/(10.0*(double)R));

    free(A);
    free(ind);
    return 0;
}

