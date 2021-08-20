#ifndef __CACHE_H__
#define __CACHE_H__


typedef struct bloque {
	char* datos;
	char valido;
	char tag;
} bloque_t;


typedef struct cache {
	unsigned short int vias;
	unsigned short int capacidad;
	bloque_t* bloques;
	unsigned short int* topes;
	unsigned short int accesos;
	unsigned short int misses;
} cache_t;


typedef struct memoria_16_bits {
	bloque_t* bloques;
	unsigned short int capacidad;
	unsigned short int tam_bloque;
} memoria_t;

//inicializar la memoria principal con capacidad ed 65 KiB, con su tamaño de bloque, y reservar el espacio de memoria para los bloques
void init_memoria(short unsigned int tam_bloque);

//inicializar la caché con su cantidad de vias y capacidad, los bloques como inválidos, los topes, misses y accesos en cero, y reservar memoria para los bloques.
void init_cache(short unsigned int vias, short unsigned int capacidad);

// inicializar los bloques de la caché como inválidos, la memoria simulada en 0 y la tasa de misses a 0
void init();

// liberar la memoria reservada por la memoria
void delete_memoria();

// liberar la memoria reservada por el cache
void delete_cache();

//devolver el conjunto de caché al que mapea la dirección address.
unsigned int find_set(int address);

//devolver el bloque más ’antiguo’ dentro de un conjunto, utilizando el campo correspondiente de los metadatos de los bloques del conjunto.
unsigned int find_earliest(int setnum);

//leer el bloque blocknum de memoria y guardarlo en el lugar que le corresponda en la memoria caché
void read_block(int blocknum);

// busca un bloque en la cache. Se fija en el conjunto correspondiente si algun bloque coincide con el tag.
// devuelve el indice del bloque en el array de bloques (la posicion en cache) si es un hit, si no devuelve basura.
unsigned short int find_block(int setnum, char* hit);

//retornar el valor correspondiente a la posición de memoria address, buscándolo primero en el caché
//escribir 1 en *hit si es un hit y 0 si es un miss
char read_byte(int address, char *hit);

// escribir el valor value en la posición correcta del bloque que corresponde a address, si está en el caché, y en la memoria en todos los casos, y
//devolver 1 si es un hit y 0 si es un miss.
char write_byte(int address, char value);

//devolver el porcentaje de misses desde que se inicializó el cache
char get_miss_rate();

#endif /* __CACHE_H__ */