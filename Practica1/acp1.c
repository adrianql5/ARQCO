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
void crearVectorRand(double *arr, int size,int D) {
    srand(time(NULL));
    for (int i = 0; i < size; i+=D) {
        double valor = 1.0 + ((double)rand()/RAND_MAX);
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

    
    double *A = (double *)aligned_alloc(64, R * D * sizeof(double)); // Reservar memoria, alineada con el comienzo de la cache
    if (A == NULL) {
        printf("Error al asignar memoria.\n");
        return 1;
    }

    crearVectorRand(A, R*D,D); // Asignar valores aleatorios al vector

    int *ind = (int *)malloc(R * sizeof(int)); // Reservar memoria para el vector de indices
    if (ind == NULL) {
        printf("Error al asignar memoria.\n");
        free(A);
        return 1;
    }

    //Dar valores a cada un dos indices
    for (int i = 0; i < R; i++) {
        ind[i] = D * i;
    }

    double suma = 0.0;
    double S[10] = {0}; // Vector S
    double ck; //Variable ciclos

    /* Medición del contador */
    start_counter();

    for (int i = 0; i < 10; i++) { // Realizar 10 veces la suma
        for (int j = 0; j < R; j++) { // Operacion de reduccion de suma
            int index = ind[j];
            suma += A[index];
        }
        S[i] = suma;
        suma = 0.0;
    }

    ck = get_counter(); // obtener contador de ciclos

    double s;
    for(int i = 0; i<10; i++){ //Sumar todos los valores de S
    	s+=S[i];
    }
    s=s/10; // Calcular valor medio de todas las medidas de S
    //printf("S: %lf\n",s); //Valor medio de S
    //printf("B:%lf\n", ck/10); // ciclos por bucle
    //printf("A:%lf\n", ck/(10*R)); //ciclos de cada acceso a memoria (A[index] se accede 10*R veces en el bucle)

    printf("%d,%d,%d,%.3lf,%.3lf,%.3lf\n", D, R, L, s, ck/10.0, ck/(10.0*R));


    //Liberar memoria
    free(A); 
    free(ind);
    return 0;
}
