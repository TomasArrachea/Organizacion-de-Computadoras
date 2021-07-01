//cache asociativa por conjuntos
//cantidad de vias, capacidad y tamaño de bloque variables
//politica de reemplazo FIFO y escritura WT/¬WA
//espacio de direcciones de 16 bits
//cada bloque de cache cuenta con su metadata, incluyendo bit V y tag, y campo que permita implementar la politica FIFO.
#include <stdlib.h>

//HACER UN TYPEDEF UNSIGNED SHORT INT y uniformizar las conversiones de tipos entre int y short int

typedef struct bloque {
	char* datos;
	char valido; //QUE SIGNIFICA? los bloques que estan en cache son validos? o solo los bloques que tienen un valor?
	char tag;
} bloque_t;

typedef struct cache {
	unsigned short int vias;
	unsigned short int capacidad;
	unsigned short int tam_bloque;
	
	unsigned short int cant_sets;
	unsigned short int cant_bloques;

	bloque_t* bloques;
	unsigned short int* topes;
	unsigned short int accesos;
	unsigned short int misses;
} cache_t;

typedef struct memoria_16_bits {
	unsigned int capacidad; // 65535 bytes, CONSTANTE
	bloque_t* bloques;
} memoria_t;

bloque_t new_bloque(int tamanio, char tag);

void init_memoria();

void init_cache(short unsigned int vias, short unsigned int capacidad, short unsigned int tam_bloque);

// inicializar los bloques de la caché como inválidos, la memoria simulada en 0 y la tasa de misses a 0
void init();

void delete();

//devolver el conjunto de caché al que mapea la dirección address.
unsigned int find_set(int address);

//devolver el bloque más ’antiguo’ dentro de un conjunto, utilizando el campo correspondiente de los metadatos de los bloques del conjunto.
// --> devuelve el indice del bloque mas antiguo en el array de bloques (la posicion en cache).
unsigned int find_earliest(int setnum);

//leer el bloque blocknum de memoria y guardarlo en el lugar que le corresponda en la memoria caché
void read_block(int blocknum);

/*usar tag para ver si cache tiene el bloque correspondiente a la address*/
// devuelve el indice del bloque en el array de bloques (la posicion en cache) si es un hit, si no devuelve basura.
unsigned short int find_block(int setnum, char* hit);

//retornar el valor correspondiente a la posición de memoria address, buscándolo primero en el caché
//escribir 1 en *hit si es un hit y 0 si es un miss
//sólo debe interactuar con la memoria a través de las otras primitivas.
char read_byte(int address, char *hit);

// escribir el valor value en la posición correcta del bloque que corresponde a address, si está en el caché, y en la memoria en todos los casos, y
//devolver 1 si es un hit y 0 si es un miss.
//sólo debe interactuar con la memoria a través de las otras primitivas.
char write_byte(int address, char value);

//devolver el porcentaje de misses desde que se inicializó el cache
char get_miss_rate();