#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

const char* VERSION_ACTUAL = "1.0";
cache_t cache;

void imprimir_ayuda();
void imprimir_version();

int main(int argc, char* argv[]) {
	FILE* salida = NULL;
	int c, ways, cache_size, block_size;

	while ((c = getopt(argc, argv, "hVowcsbs:")) != -1)
		switch (c) {
			case 'h':
				imprimir_ayuda();
			    return 0;
			case 'V':
				imprimir_version();
			    return 0;
			case 'o':
			    nombre_salida = optarg;
			    break;
			case 'w':
			    ways = optarg;
			    break;
			case 'cs':
			    cache_size = optarg;
			    break;
			case 'bs':
			    block_size = optarg;
			    break;
			case '?':
			    if (optopt == 'o')
					fprintf (stderr, "La opción -%c requiere un argumento.\n", optopt);
			    else if (isprint (optopt))
			    	fprintf (stderr, "Opción desconocida `-%c'.\n", optopt);
			    else
			    	fprintf (stderr,"Caracter desconocido `\\x%x'.\n", optopt);
			    return 1;
			default:
			    abort ();
    }  
    int index = optind;

    init();

	return 0;
}

void imprimir_ayuda(){
	printf(
	"	Uso:\n"
	"		tp2 -h\n"
	"		tp2 -V\n"
	"		tp2 options archivo\n"
	"\n	Opciones:\n"
	"		-h	Imprime este mensaje.\n"
	"		-V 	Da la versión del programa.\n"
	"		-s 	Imprime por consola la salida del programa, en lugar de guardarlo en el archivo de salida.\n"
	"		-o 	Prefijo de los archivos de salida.\n"
	"		-w	Cantidad de vı́as.\n"
	"		-cs Tamaño del caché en kilobytes.\n"
	"		-bs Tamaño de bloque en bytes.\n"
	"\n	Ejemplos:\n"
	"	-> tp2 -w 4 -cs 8 -bs 16 prueba1.mem\n");
}

void imprimir_version(){
	printf("Versión actual: %s\n", VERSION_ACTUAL);
}