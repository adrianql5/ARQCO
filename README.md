# ARQCO - Prácticas de Arquitectura de Computadores

Repositorio de prácticas desarrolladas por **Adrián Quiroga Linares** y **Xabier Novoa Gómez** para la asignatura de **Arquitectura de Computadores**. Aquí se exploran aspectos clave del rendimiento computacional y comportamiento de memoria mediante programación en C y técnicas de análisis de bajo nivel.

---

# Práctica 1: Accesos a Memoria y Efectos de Caché

## Objetivo
Analizar el coste temporal de accesos a memoria variando el patrón y tamaño de acceso, con el fin de observar los efectos de la jerarquía de cachés del procesador (localidad espacial, temporal y técnicas de *prefetching*).

## Implementación
- Lenguaje: C
- Se reserva memoria dinámica para un vector de tipo `double`, al que se accede de forma indirecta mediante un vector de índices `ind[]`.
- Se realiza una operación de reducción de suma sobre elementos del tipo `A[ind[i]]`, repitiendo la operación 10 veces para evitar optimizaciones del compilador.
- Se miden los ciclos de CPU usando rutinas específicas provistas para tal fin.
- Se estudia el comportamiento para distintos valores de:
  - `R`: número de accesos, ajustado en función del tamaño de las cachés L1 y L2.
  - `D`: distancia entre accesos, usando potencias de 2 para analizar localidad.

## Experimentos Adicionales
- Comparación del rendimiento al usar:
  - Tipos `int` en vez de `double`.
  - Accesos directos en lugar de indirectos vía `ind[]`.

---

## Práctica 2: Método de Jacobi y Técnicas de Optimización

## Objetivo
Implementar y analizar el método iterativo de **Jacobi** para resolver sistemas de ecuaciones lineales, evaluando su rendimiento bajo distintas optimizaciones.

## Versiones Implementadas
1. **Secuencial Base:** Código sin optimización, implementación directa del pseudocódigo.
2. **Secuencial Optimizado (Caché):**
   - Fusión/división de bucles.
   - Desenrollamiento de bucles.
   - Computación por bloques.
3. **SIMD (AVX256):**
   - Uso de instrucciones vectoriales mediante `#include <immintrin.h>`.
4. **Paralelo (OpenMP):**
   - Variación del número de hilos.
   - Distintos tipos de *scheduling*.
   - Reducciones, secciones críticas y atómicas.

##  Condiciones de los Experimentos
- Ejecución en **FinisTerrae III** (CESGA).
- Se prueba para tamaños `n = 250, 2500, 5000`.
- Se fija `tol = 1e-8` y `max_iter = 20000`.
- Inicialización aleatoria controlada:
  - `srand(n)`, valores en `(float) rand() / RAND_MAX`.
  - Diagonal dominante para asegurar convergencia.

## Medición del Rendimiento
- Se mide únicamente la parte computacional (no se cuenta la impresión por pantalla).
- Para cada configuración se hacen 15 medidas, seleccionando la **mediana**.


## Compilación y Ejecución

Ejemplo de compilación con GCC:
```bash
gcc -O2 -msse3 -o practica1 practica1.c
gcc -O2 -fopenmp -mavx -o jacobi_simd jacobi_simd.c
```


---
# Autores

- Adrián Quiroga Linares  
- Xabier Novoa Gómez

Trabajo desarrollado en el marco de la asignatura **Arquitectura de Computadores**, curso 2024–2025.


¿Te gustaría que prepare también una versión en inglés, o quieres añadir gráficos/resultados al READ
