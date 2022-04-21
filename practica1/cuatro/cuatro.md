# El jardín ornamental


## Explicación del problema
La instrucción ```visitantes++;``` no es atómica. Si la convertimos en assembler del estándar x86 de intel tenemos:
```
mov $visitantes %rax #Leer
add $1 %rax #Incrementar
mov %rax $visitantes #Guardar
```
Al ejecutarse el programa el sistema operativo le quitará y dará el control a los procesos alternandolos.
Cuando se interrumpe un proceso los valores almacenados en registros se preservan (junto con la información sobre el proceso) para poder restaurarlo cuando la ejecución continúe. Así, cada molinete tiene su propia copia de los valores en los registros. Sin embargo, el resto de la memoria es compartida (se utiliza ese hecho para llevar la cuenta de personas que ingresan). 
En este trabajo no podemos suponer que las interrupciones serán de la manera esperada.

Por lo que puede darse la siguiente secuencia de eventos durante la ejecución de las instrucciones:

* visitantes = 0

* molinete1: LEER (rax1 = 0, visitantes = 0) 

* molinete1: INCREMENTAR (rax1 = 1, visitantes = 0)

* El sistema operativo decide cambiar de proceso,suspende molinete1 y continúa con molinete2.

* molinete2: LEER (rax2 = 0, visitantes = 0)

* molinete2: INCREMENTAR (rax2 = 1, visitantes = 0)

* molinete2: GUARDAR (rax2 = 1, visitantes = 1)

* El sistema operativo decide cambiar de proceso, suspende molinete2 y continúa con molinete1.

* molinete1: GUARDAR (rax1 = 1, visitantes = 1)

Ambos procesos ejecutaron sus instrucciones para incrementar en 1 el contador, sin embargo, el primer proceso leyó antes de que el segundo incrementara el contador y así se perdió uno en la suma.

## ¿Qué tan probable es que falle?
Usualmente un sistema operativo ejecuta miles de instrucciones antes de cambiar de un proceso a otro, por lo que si el numero de visitantes es menor, hay menos probabilidad de que ocurra la secuencia anterior.

## ¿Cuál es el mínimo valor que podría imprimir el programa?
En el peor de los casos se podría dar la siguiente secuencia:

* visitantes = 0

* molinete1: LEER (rax1 = 0, visitantes = 0) 

* molinete1: INCREMENTAR (rax1 = 1, visitantes = 0)

* El sistema operativo decide cambiar de proceso, suspende molinete1 y continúa con molinete2.

* molinete2: LEER (rax2 = 0, visitantes = 0)

* molinete2: INCREMENTAR (rax2 = 1, visitantes = 0)

* molinete2: GUARDAR (rax2 = 1, visitantes = 1)

* ... sigue el molinete2 N_VISITANTES-1 veces ...

* molinete2: GUARDAR (rax2 = N_VISITANTES-1, visitantes = N_VISITANTES-1)

* El sistema operativo decide cambiar de proceso, suspende molinete2 y continúa con molinete1.

* molinete1: GUARDAR (rax1 = 1, visitantes = 1)

* El sistema operativo decide cambiar de proceso, suspende molinete1 y continúa con molinete2.

* molinete2: LEER (rax2 = 1, visitantes = 1)

* molinete2: INCREMENTAR (rax2 = 2, visitantes = 1)

* El sistema operativo decide cambiar de proceso, suspende molinete2 y continúa con molinete1.

* molinete1: LEER (rax1 = 1, visitantes = 1)

* ... sigue el molinete1 N_VISITANTES-1 veces ...

* molinete1: GUARDAR (rax1 = N_VISITANTES, visitantes = N_VISITANTES) (motinete1 termina)

* molinete2: GUARDAR (rax2 = 2, visitantes = 2) (molintete2 termina)

Por lo tanto, el mínimo valor que podría imprimir el programa es 2.