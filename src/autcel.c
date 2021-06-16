#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

const char* VERSION_ACTUAL = "1.2";
const int MULTIPLICADOR_PIXELS = 4;

extern unsigned char proximo(unsigned char* a, unsigned int i, unsigned int j, unsigned char regla, unsigned int n);
void imprimir_ayuda();
void imprimir_version();
int cargar_inicio(char* nombre_inicial, unsigned char** estados, int n);
void guardar_fila(FILE* salida, unsigned char* estados, int n);
FILE* inicializar_pbm(char* nombre, int n);
void generar_automata(FILE* salida, char* nombre_inicial, char regla, int n);

int main(int argc, char* argv[]) {
	FILE* salida = NULL;
	int c;
	char* nombre_salida = "evolucion.pbm";

	while ((c = getopt(argc, argv, "hVpo:")) != -1)
		switch (c) {
			case 'h':
				imprimir_ayuda();
			    return 0;
			case 'V':
				imprimir_version();
			    return 0;
			case 'p':
				salida = stdout;
				break;
			case 'o':
			    nombre_salida = optarg;
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

    if (argc < 4) {
		perror("Error: los argumentos son inválidos.");
		return 1;
    }

	int regla = atoi(argv[index++]);
    int n = atoi(argv[index++]);
	char* nombre_inicial = argv[index++];

    if (salida == NULL) {
		salida = inicializar_pbm(nombre_salida, n);
	}

	if (regla > 255 || regla <= 0 || n <= 0) {
		perror("Error: los argumentos son inválidos.");
		fclose(salida);
		return 1;
	}
	
	generar_automata(salida, nombre_inicial, (char)regla, n);

	fclose(salida);
	return 0;
}

FILE* inicializar_pbm(char* nombre, int n) {
	FILE* file = fopen(nombre, "w");
	if (!file){
		perror("Error al crear el archivo de salida.");
		return 0;
    }

	fprintf(file, "P1\n# feep.pbm\n%i %i\n", n*MULTIPLICADOR_PIXELS, n*MULTIPLICADOR_PIXELS);

	return file;
}

void generar_automata(FILE* salida, char* nombre_inicial, char regla, int n) {
	unsigned char* estados[n];
	for (int i = 0; i < n; i++)
		estados[i] = malloc(n*sizeof(char));

	if (cargar_inicio(nombre_inicial, estados, n) != 0) {
		return;		
	}

	guardar_fila(salida, estados[0], n);
	for (unsigned int i = 0; i < n-1; i++) {
		for (unsigned int j = 0; j < n; j++) {
			estados[i+1][j] = proximo((unsigned char*)estados, i, j, regla, n);
		}
		guardar_fila(salida, estados[i+1], n);
	}

	for (int j = 0; j < n; j++)
		free(estados[j]);			
}

void imprimir_ayuda(){
	printf(
	"	Uso:\n"
	"		autcel -h\n"
	"		autcel -V\n"
	"		autcel R N inputfile -s\n"
	"		autcel R N inputfile [-o outputprefix]\n"
	"\n	Opciones:\n"
	"		-h	Imprime este mensaje.\n"
	"		-V 	Da la versión del programa.\n"
	"		-s 	Imprime por consola la salida del programa, en lugar de guardarlo en el archivo de salida.\n"
	"		-o 	Prefijo de los archivos de salida.\n"
	"\n	Ejemplos:\n"
	"	-> autcel 30 80 inicial -o evolucion\n"
	"		Calcula la evolución del autómata 'Regla 30', en un mundo unidimensional de 80 celdas, por 80 iteraciones.\n"
	"		El archivo de salida se llamará evolucion.pbm.\n"
	"		Si no se da un prefijo para los archivos de salida, el prefijo será el nombre del archivo de entrada\n");
}

void imprimir_version(){
	printf("Versión actual: %s\n", VERSION_ACTUAL);
}

int cargar_inicio(char* nombre_inicial, unsigned char** estados, int n) {
	FILE* archivo_inicial = fopen("inicial", "r");
	if (!archivo_inicial){
		perror("Error al abrir el archivo inicial.");
		return 2;
	}

	char numero;
	for (int j = 0; j < n; j++){
		numero = getc(archivo_inicial) - '0';
		if (numero == EOF){
			fclose(archivo_inicial);
			perror("Error: el archivo inicial tiene celdas de menos.");
			return -1;

		} else if (numero != 0 && numero != 1){
			fclose(archivo_inicial);
			perror("Error: el archivo inicial no cumple con el formato.");
			return -1;
		}
		estados[0][j] = numero;
	}

	if (fscanf(archivo_inicial, "%s", &numero) != EOF){
		perror("Error: el archivo inicial tiene celdas de sobra.");
		return 1;
	}

	fclose(archivo_inicial);

	return 0;
}

void guardar_fila(FILE* salida, unsigned char* estados, int n){
	for (int k = 0; k < MULTIPLICADOR_PIXELS; k++)	{
		for (int j = 0; j < n; j++)	{
			for (int i = 0; i < MULTIPLICADOR_PIXELS; i++) {
				fprintf(salida, "%i ", estados[j]);
			}
		}
		fprintf(salida, "\n");
	}
}