#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include "cache.h"

const char* VERSION_ACTUAL = "1.0";

void imprimir_ayuda();
void imprimir_version();
void leer_archivo(char* nombre);
void ejecutar_instrucciones(char* archivo_instrucciones, char* archivo_salida);
void parse_arguments(int argc, char** argv, int* ways, int* cache_size, int* block_size, char* archivo_salida, char* archivo_instrucciones);


int main(int argc, char* argv[]) {
	FILE* salida = NULL;
	int ways = 4, cache_size = 4096, block_size = 256;
	char* archivo_salida = "resultado_cache.mem", *archivo_instrucciones;

	parse_arguments(argc, argv, &ways, &cache_size, &block_size, archivo_salida, archivo_instrucciones);
	init_cache(ways, cache_size, block_size);
    ejecutar_instrucciones(archivo_instrucciones, archivo_salida);
    delete();
	return 0;
}


void parse_arguments(int argc, char** argv, int* ways, int* cache_size, int* block_size, char* archivo_salida, char* archivo_instrucciones) {
	int c;
    static struct option long_options[] = {
        {"help",    no_argument,      0,  'h' },
        {"version",	no_argument,       0,  'v' },
        {"output",  required_argument, 0,  'o' },
        {"ways",	required_argument, 0,  'w' },
        {"cachesize", required_argument, 0,  'c' },
        {"blocksize", required_argument, 0,  'b' },
        {"cs", required_argument, 0,  'c' },
        {"bs", required_argument, 0,  'b' },
        {0,           0,                 0,  0   }
    };

	while ((c = getopt_long_only(argc, argv, "hVo:w:c:b:", long_options, NULL)) != -1) //usar longopt para poder manear opciones de varios caracteres
		switch (c) {
			case 'h':
				imprimir_ayuda();
			    return;
			case 'V':
				imprimir_version();
			    return;
			case 'o':
			    archivo_salida = optarg;
			    break;
			case 'w':
			    *ways = atoi(optarg);
			    break;
			case 'c':
			    *cache_size = atoi(optarg);
			    break;
			case 'b':
			    *block_size = atoi(optarg);
			    break;
			case '?':
			    if (optopt == 'o' || optopt == 'c' || optopt == 'b' || optopt == 'w')
					fprintf (stderr, "La opción -%c requiere un argumento.\n", optopt);
			    else if (isprint (optopt))
			    	fprintf (stderr, "Opción desconocida `-%c'.\n", optopt);
			    else
			    	fprintf (stderr,"Caracter desconocido `\\x%x'.\n", optopt);
			    return;
			default:
				abort();
				// fprintf (stderr, "Error: los argumentos son inválidos. Ingrese -h para ver la ayuda.")
    }
    int index = optind;
    if (argc < 2) {	
		fprintf (stderr, "Error: se debe ingresar el nombre de un archivo con instrucciones. Ingrese -h para ver la ayuda.");
		return;
    }
	archivo_instrucciones = argv[index++];
}

void ejecutar_instrucciones(char* archivo_instrucciones, char* archivo_salida) {
	FILE* instrucciones = fopen(archivo_instrucciones, "r");
	if (!instrucciones){
		fprintf(stderr, "Error al abrir el archivo de instrucciones.");
		return;
	}
	FILE* salida = fopen(archivo_salida, "r");
	if (!salida){
		fprintf(stderr, "Error al abrir el archivo de resultados.");
		return;
	}

	char comando;
	while(!feof(instrucciones)) {
		fscanf(instrucciones, "%c", &comando);
		switch (comando) {
			case 'W' : {
				int address;
				char value;
				fscanf(instrucciones, "%i, %c\n", &address, &value);
				char hit = write_byte(address, value);
				fprintf(salida, "Write value %c in address %i: hit = %c\n", value, address, hit);
				break;
			}
			case 'R': {
				int address;
				fscanf(instrucciones, "%i\n", &address);
				char hit;
				char value = read_byte(address, &hit);
				fprintf(salida, "Read value %c from address %i: hit = %c\n", value, address, hit);
				break;
			}
			case 'M':
				fscanf(instrucciones, "R\n");
				fprintf(salida, "Miss rate: %i\n", get_miss_rate());
				break;
			case 'i':
				fscanf(instrucciones, "nit\n");
				init();
				fprintf(salida, "init\n");
				break;
			default:
				return;
		}
	}

	fclose(instrucciones);
	fclose(salida);
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