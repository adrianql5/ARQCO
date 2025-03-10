# Introducción
Este trabajo tiene como objetivo caracterizar el coste temporal asociado a la lectura (acceso) de datos mediante la implementación de un programa en lenguaje C. Se analizará cómo varía este coste al modificar distintos parámetros en el patrón de acceso y el tamaño del conjunto de datos. Además, se observará e interpretará el impacto del sistema de memorias caché del microprocesador, considerando los principios fundamentales de localidad espacial y temporal. También se explorará el efecto de las técnicas de precarga (prefetching) de datos, con el fin de comprender mejor cómo optimizar el acceso a la memoria y mejorar el rendimiento del sistema.

# Datos sobre la caché
Caché multilevel de 3 niveles asociativa por conjuntos.

$TamLinea = 64 bytes$

**Caché L1 de Datos:**
$TamL1Datos = 48 Kbytes$
$nConjuntosL1Datos = 12$
$nLineasL1Datos = 768$ 

**Caché L2:**
$TamL2 = 1280 Kbytes$
$nConjuntosL2 = 20$
$nLineasL2 = 20480$ 

**De donde sacamos la información**
``` Bash
ls /sys/devices/system/cpu/cpu0/cache/index0/ #cache L1Datos

ls /sys/devices/system/cpu/cpu0/cache/index0/ #cache L2

#Hacemos un cat a cada archivo
```


# Variables del programa
$R = nºElementos$ 
$D = desplazamiento$
$L = nºLineasCaché$

Si queremos usar $L$ lineas, obtenemos la siguiente expresión (*teniendo en cuenta el caso dónde usamos doubles*):
$$ L \times 64 = R \times 8 \times D$$
Como conocemos tanto $L$ como $D$, despejamos para $R$:
$$ R = \frac{L\times64}{D\times8}$$

$$ R = \frac{L\times8}{D}$$
Además si $D \geq 8$ , porque si no no accederíamos al número de líneas especificado: 
$$R = L$$
Por ejemplo si tenemos $L=2$ y $D=16$ nos da que $R=1$, y estaríamos guardando un elemento en la línea, pero solo uno. Al hacer $R=L$ obligamos a que sustituya toda la linea y explore otra por lo que visita las 2 lineas que queremos en este caso.

Tenemos un array que contiene los índices y otro array de elementos tal que:
```C
ind[R-1]
A[R*D-1]
```

Los elementos de $ind$ siguen la secuencia $[0, D, 2D,...,(R-1)D]$
Y los elementos de $A$ son valores double con valor acotado entre el intervalo $[1,2)$ con signo o negativo o positivo.

# Resultados Double

| **D   \|   L** | 384     | 1152   | 10240   | 15360   | 40960   | 81920   | 163840  |
| -------------- | ------- | ------ | ------- | ------- | ------- | ------- | ------- |
| **2**          | 7,1402  | 7,1724 | 7,1359  | 7,1619  | 7,2115  | 7,2024  | 7,1758  |
| **8**          | 7,3884  | 7,1452 | 7,1936  | 7,2565  | 7,3335  | 7,2445  | 7,2365  |
| **128**        | 7,1901  | 7,7563 | 7,5834  | 8,0227  | 15,488  | 15,7873 | 16,0443 |
| **512**        | 10,6048 | 8,2842 | 14,5713 | 16,4789 | 16,8837 | 17,2648 | 17,0103 |
| **1024**       | 9,9473  | 8,1455 | 17,9781 | 17,9506 | 17,8761 | 17,9533 | 17,9259 |

![[Pasted image 20250223113145.png]]

**Analisis de los resultados**

Como comentabamos antes en una línea caché en nuestro caso cogen 8 valores double, por lo que cada vez que aumentemos el valor de D, dismunuirá el número de valores del vector que vamos a almacenar en cada línea caché. Esto provoca que no se pueda aprovechar el **principio de localidad**, ya que se gastan más ciclos de reloj en llegar a la información. Como se observa en la gráfica para valores menores e iguales que 8, los ciclos necesarios para acceder a un elemento se mantienen prácticamente entre los mismos ciclos de reloj. Al aumentar el valor de D, los ciclos por acceso también aumentan
# Resultados Int

En este caso, los números enteros ocupan menos espacio (4 bytes en nuestra máquina), por lo que ahora en una linea caché van a coger 16 valores. Se observa la misma tendencia que crecimiento que en el caso anterior, ya que un aumento de D, provoca más ciclos de reloj para acceder a toda la información. 

| D   \|    L | 384    | 1152   | 10240   | 15360   | 40960   | 81920   | 163840  |
| ----------- | ------ | ------ | ------- | ------- | ------- | ------- | ------- |
| 4           | 5,8912 | 5,9476 | 5,9414  | 5,9676  | 6,0558  | 5,9766  | 5,9764  |
| 128         | 6,5445 | 6,5568 | 7,6753  | 7,5618  | 9,0387  | 15,5896 | 16,9441 |
| 512         | 6,9391 | 9,1525 | 9,0761  | 14,2025 | 16,5632 | 16,8165 | 16,8279 |
| 1024        | 8,7839 | 8,5833 | 15,0301 | 16,5749 | 17,4921 | 17,2882 | 17,4355 |
| 2048        | 8,8888 | 8,7003 | 17,2736 | 17,8446 | 18,1078 | 18,2216 | 18,204  |
![[Salida2.png]]


**Comparación Double-Integer**

Para un mismo número de elementos, se va a ocupar menos espacio, por lo tanto el procesador tiene que acceder a menos líneas de caché lo que se traduce en menos ciclos de reloj por acceso. En el caso con D = 2 es bastante apreciable la diferencia:

![[Pasted image 20250223120953.png]]


En valores de D por encima de 16 el números de ciclos de por acceso de integers y Doubles sigue la misma tendencia de crecimiento, con prácticamente los mismos valores
# Resultados referencias directas a los elementos de $A$

En los casos anteriores estábamos utilizando un array de índices para acceder a las posiciones del array A, pero ahora vamos a acceder directamente, como consecuencia se puede observar una reducción de los ciclos por acceso
![[Pasted image 20250223123831.png]]

| **D   \|    L** | 384    | 1152   | 10240   | 15360   | 40960   | 81920   | 163840  |
| --------------- | ------ | ------ | ------- | ------- | ------- | ------- | ------- |
| **2**           | 5,8883 | 5,9127 | 5,9063  | 5,9073  | 5,9315  | 5,9253  | 5,9212  |
| **8**           | 5,9861 | 5,9161 | 6,0205  | 6,015   | 6,1244  | 6,1068  | 6,0654  |
| **128**         | 6,524  | 6,5778 | 7,8535  | 8,0543  | 9,116   | 15,3613 | 16,834  |
| **512**         | 7,0826 | 8,6855 | 9,6305  | 14,3376 | 16,3163 | 16,7335 | 16,829  |
| **1024**        | 8,897  | 8,5429 | 15,0764 | 16,5688 | 16,8449 | 17,1431 | 17,2188 |
# Interpretación de Resultados
### Prefetching
El **prefetching** o **precarga de datos** es una técnica utilizada por los microprocesadores para mejorar el rendimiento, anticipando las necesidades de datos antes de que sean requeridos por los programas. En otras palabras, el procesador predice qué datos necesitará en el futuro cercano y los carga en la caché de manera proactiva. Esto reduce el tiempo de espera cuando esos datos realmente sean necesarios, ya que en lugar de tener que esperar a que se carguen desde la memoria principal (que es más lenta), el procesador puede acceder a ellos de manera mucho más rápida desde la caché.

El **prefetching** se apoya en los conceptos de **localidad temporal** y **localidad espacial**:
- **Localidad temporal**: Si un dato se ha accedido recientemente, es probable que se vuelva a acceder en un futuro cercano. El procesador puede predecir que si ya se usaron ciertos datos, es probable que se necesiten nuevamente pronto.
- **Localidad espacial**: Si un dato se ha accedido en una dirección de memoria, es probable que se acceda a otros datos cercanos. Esto permite al procesador cargar no solo el dato que se necesita, sino también otros cercanos que podrían ser utilizados.

El procesador del cesga que utilizamos para nuestras pruebas (Intel Xeon Ice Lake
8352Y) utiliza el prefetching por defecto en la LLC

## **Funcionamiento del HW LLC Prefetch**

1. **Predicción de accesos futuros**
    
    - El hardware monitorea los accesos de memoria realizados por el procesador.
    - Si detecta un **patrón regular** en los accesos a memoria (por ejemplo, acceso a datos consecutivos con una cierta separación o stride), anticipa futuras solicitudes y pre-carga los datos en la LLC.
2. **Carga en la LLC**
    
    - Una vez detectado el patrón, el prefetcher **trae líneas de caché** desde la memoria DRAM o HBM hacia la LLC antes de que sean realmente solicitadas.
    - Esto reduce la latencia de acceso a memoria y previene bloqueos del procesador causados por esperas en la recuperación de datos.
3. **Dependencia del patrón de acceso**
    
    - **Si el tráfico de memoria tiene strides pequeños y predecibles**, el LLC prefetch es muy efectivo y puede acelerar el rendimiento de las aplicaciones.
    - **Si el tráfico de memoria es irregular o tiene strides grandes**, el prefetcher puede traer datos innecesarios, lo que incrementa el consumo de ancho de banda y afecta negativamente el rendimiento.

Fuente:https://www.intel.la/content/www/xl/es/content-details/780991/hardware-llc-prefetch-feature-on-4th-gen-intel-xeon-scalable-processor-codename-sapphire-rapids.html



