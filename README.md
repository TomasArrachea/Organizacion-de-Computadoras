# TP1-MIPS
Se trata de una versión en lenguaje C de un programa que computa
autómatas celulares para reglas arbitrarias. El programa recibirá como
argumentos el número de regla R, la cantidad de celdas de una fila N y
el nombre de un archivo de texto con el contenido del estado inicial del
autómata, y escribirá un archivo .PBM [5] representando la evolución del
autómata celular en una matriz de N xN . El archivo de estado inicial debe
contener una lı́nea con N dı́gitos binarios, con 1 representando una celda
ocupada y 0 una vacı́a. De haber errores, los mensajes de error deberán salir
exclusivamente por stderr.