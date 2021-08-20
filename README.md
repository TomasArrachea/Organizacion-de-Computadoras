## TP1-MIPS
Se trata de una versión en lenguaje C de un programa que computa
autómatas celulares para reglas arbitrarias. El programa recibirá como
argumentos el número de regla R, la cantidad de celdas de una fila N y
el nombre de un archivo de texto con el contenido del estado inicial del
autómata, y escribirá un archivo .PBM representando la evolución del
autómata celular en una matriz de N xN . El archivo de estado inicial debe
contener una lı́nea con N dı́gitos binarios, con 1 representando una celda
ocupada y 0 una vacı́a. De haber errores, los mensajes de error deberán salir
exclusivamente por stderr.

## TP2-memoria-caché
La memoria a simular es una caché asociativa por conjuntos, en que
se puedan pasar por parámetro el número de vı́as, la capacidad y el tamaño
de bloque. La polı́tica de reemplazo será FIFO y la polı́tica de escrituraserá 
WT/¬WA. Se asume que el espacio de direcciones es de 16 bits, y hay
entonces una memoria principal a simular con un tamaño de 64KB.

## TP3-pipelines
El objetivo de este trabajo es familiarizarse con la arquitectura de una CPU MIPS, especı́fi-
camente con el datapath y la implementación de instrucciones. Para ello, se deberán agregar
instrucciones a diversas configuraciones de CPU provistas por el simulador DrMIPS

