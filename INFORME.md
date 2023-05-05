Preguntas:

1. Que diferencia observa entre el caso de estudio 1 y 2? Cual es la fuente limitante en cada uno? Investigue sobre la diferencia entre control de flujo y control de congestion (Figura 6-22 Tanenbaum)

En el caso de estudio 2 se modela la situacion donde tenemos una subred con una alta capacidad para producir (NodeTx), conectada a otra subred consumidora con poca capacidad de carga (Queue). Esto es un problema de Congestion. Queue no tiene la suficiente capacidad de carga para procesar lo que NodeTx le manda. En este caso la perdida de paquetes por congestion ocurrira en Queue, llegara un punto en el que el modulo queue, que entrega los paquetes a Sink, no podra almacenar mas paquetes en su buffer, lo que resulta en lo antes descripto.
Por otro lado en el caso de estudio 2 se presenta un problema de control de flujo.

1. Como cree que se comporta su algoritmo de control de flujo y congestion? Funciona para el caso de estudio 1 y 2 por igual? Por que?

 En caso de implementar control de flujo y control en una sola estrategia, se recomienda evaluar el
sistema con un tamaño de buffer de 100 paquetes en la queue conectando el transmisor y receptor. Este
escenario permitirá estudiar el algoritmo con ambas funcionalidades operando simultáneamente.

