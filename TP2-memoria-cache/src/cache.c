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


cache_t cache;
memoria_t memoria;


bloque_t new_bloque(int tamanio, char tag) {
	bloque_t bloque;
	bloque.valido = 0; //son validos los bloques de la memoria? no y se validan cuando se les escribe un dato
	bloque.tag = tag;
	bloque.datos = malloc(tamanio * sizeof(char));
	// for (int i = 0; i < tamanio; ++i) { bloque.datos[i] = 0; } hace falta inicializar en 0 todos los datos? podrian ser basura invalida
	return bloque;
}

void init_memoria() {
	int cant_bloques_memoria = (memoria.capacidad / cache.tam_bloque);
	memoria.capacidad = 0xffff;
	memoria.bloques = malloc(cant_bloques_memoria * sizeof(bloque_t));

	for (int i = 0; i < cant_bloques_memoria; i++) {
		memoria.bloques[i] = new_bloque(cache.tam_bloque, i / cache.vias);
	}
}

void init_cache(short unsigned int vias, short unsigned int capacidad, short unsigned int tam_bloque) {
	cache.vias = vias;
	cache.capacidad = capacidad;
	cache.tam_bloque = tam_bloque;

	cache.cant_sets = capacidad / (tam_bloque * vias);
	cache.cant_bloques = capacidad / tam_bloque;

	cache.bloques = malloc(cache.cant_bloques * sizeof(bloque_t));
	for (int i = 0; i < cache.cant_bloques; i++) {
		cache.bloques[i] = new_bloque(tam_bloque, i / cache.vias);
		// cache.bloques[i].valido = 0; // no hay bloques inicializados, SEGFAULT?
	}
	for (int i = 0; i < cache.cant_sets; i++) {
		cache.topes[i] = 0;
	}
	cache.misses = 0;
	cache.accesos = 0;

	init_memoria();
}

// inicializar los bloques de la caché como inválidos, la memoria simulada en 0 y la tasa de misses a 0
void init() {
	for (int i = 0; i < cache.cant_bloques; i++) {
		cache.bloques[i].valido = 0;
	}
	cache.misses = 0;
	cache.accesos = 0;

	int cant_bloques_memoria = (memoria.capacidad / cache.tam_bloque);
	for (int i = 0; i < cant_bloques_memoria; i++) {
		memoria.bloques[i].valido = 0;
		for (int j = 0; j < cache.tam_bloque; j++) {
			memoria.bloques[i].datos[j] = 0;
		}
	}
}

void delete() {
	int cant_bloques_memoria = (memoria.capacidad / cache.tam_bloque);
	for (int i = 0; i < cant_bloques_memoria; i++) {
		free(memoria.bloques[i].datos);
	}
	free(memoria.bloques);

	for (int i = 0; i < cache.cant_bloques; i++) {
		free(cache.bloques[i].datos);
	}
	free(cache.bloques);
}

//devolver el conjunto de caché al que mapea la dirección address.
unsigned int find_set(int address) {
	return (address/cache.tam_bloque) % cache.cant_sets;
}

//devolver el bloque más ’antiguo’ dentro de un conjunto, utilizando el campo correspondiente de los metadatos de los bloques del conjunto.
// --> devuelve el indice del bloque mas antiguo en el array de bloques (la posicion en cache).
unsigned int find_earliest(int setnum) {
	return setnum * cache.vias + cache.topes[setnum];
}

//leer el bloque blocknum de memoria y guardarlo en el lugar que le corresponda en la memoria caché
void read_block(int blocknum) {
	memoria.bloques[blocknum].valido = 1;
	unsigned int setnum = find_set(blocknum * cache.tam_bloque);
	cache.bloques[find_earliest(setnum)] = memoria.bloques[blocknum];

	//FALTA GUARDAR EL TAG ACA, o cuando los inicializo en memoria?
	if (cache.topes[setnum] >= cache.vias - 1)
		cache.topes[setnum] = 0;
	else 
		cache.topes[setnum]++;
}

//escribir el valor value en la memoria
void write_byte_tomem(int address, char value) {
	unsigned int block_offset = address & ~(0xffff / cache.cant_bloques); // si la cant de bloques no es potencia de 2 pasan cosas raras, es mejor usar >> log2(cant_bloques), o ver que pasa usando %, como find_set
	memoria.bloques[address / cache.tam_bloque].datos[block_offset] = value;
}

/*usar tag para ver si cache tiene el bloque correspondiente a la address*/
// devuelve el indice del bloque en el array de bloques (la posicion en cache) si es un hit, si no devuelve basura.
unsigned short int find_block(int setnum, char* hit){
	short int i = 0, blocknum;
	*hit = 0;
	while (!(*hit) && i < cache.vias) {
		blocknum = setnum * cache.vias + i;
		if (cache.bloques[blocknum].tag == setnum) {
			*hit = 1;
		}
	}
	return blocknum;
}

//retornar el valor correspondiente a la posición de memoria address, buscándolo primero en el caché
//escribir 1 en *hit si es un hit y 0 si es un miss
//sólo debe interactuar con la memoria a través de las otras primitivas.
char read_byte(int address, char *hit) {
	unsigned short int blocknum = find_block(find_set(address), hit);
	if (*hit == 0) {
		read_block(address / cache.tam_bloque);
		blocknum = find_block(find_set(address), hit);
		cache.misses++;
	}
	cache.accesos++;
	unsigned int block_offset = address & ~(0xffff / cache.cant_bloques); // si la cant de bloques no es potencia de 2 pasan cosas raras, es mejor usar >> log2(cant_bloques), o ver que pasa usando %, como find_set
	return cache.bloques[blocknum].datos[block_offset];
}

// escribir el valor value en la posición correcta del bloque que corresponde a address, si está en el caché, y en la memoria en todos los casos, y
//devolver 1 si es un hit y 0 si es un miss.
//sólo debe interactuar con la memoria a través de las otras primitivas.
char write_byte(int address, char value){
	char hit;
	unsigned short int blocknum = find_block(find_set(address), &hit);
	cache.accesos++;

	if (hit == 1) {
		unsigned int block_offset = address & ~(0xffff / cache.cant_bloques); // si la cant de bloques no es potencia de 2 pasan cosas raras, es mejor usar >> log2(cant_bloques), o ver que pasa usando %, como find_set
		cache.bloques[blocknum].datos[block_offset] = value;
	} else
		cache.misses++;

	write_byte_tomem(address, value);
	return hit;
}

//devolver el porcentaje de misses desde que se inicializó el cache
char get_miss_rate() {
	return cache.misses / cache.accesos;
}