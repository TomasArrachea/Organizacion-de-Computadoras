//cache asociativa por conjuntos
//cantidad de vias, capacidad y tamaño de bloque variables
//politica de reemplazo FIFO y escritura WT/¬WA
//espacio de direcciones de 16 bits
//cada bloque de cache cuenta con su metadata, incluyendo bit V y tag, y campo que permita implementar la politica FIFO.

#ifndef __CACHE_H__
#define __CACHE_H__

//UNIFORMIZAR LENGUAJE, atributos en español pero funciones en ingles

typedef struct memoria_16_bits {
	const unsigned int CAPACIDAD = 0xffff // 65535 bytes
	char bloques[CAPACIDAD]; //podria ser un array de bloques, seteados como invalidos.
} memoria_t;

typedef struct cache {
	unsigned int vias;
	unsigned int capacidad;
	unsigned int tam_bloque;
	
	unsigned int tam_set = tam_bloque * vias;
	unsigned int cant_sets = capacidad / (tam_bloque * vias);
	unsigned int cant_bloques =  capacidad / tam_bloque;

	bloque_t bloques[cant_bloques];
	unsigned int topes[cant_sets];
	unsigned int accesos;
	unsigned int misses;
} cache_t;

typedef struct bloque {
	unsigned int datos[tam_bloque];
	char valido;
	char tag;
} bloque_t;

// inicializar los bloques de la caché como inválidos, la memoria simulada en 0 y la tasa de misses a 0
void init() {
	//metadatos del cache ya inicializados? vias, capacidad, cant_sets, topes
	for (int i = 0; i < cache.cant_sets; i++) {
		tope[i] = 0;
	}
	for (int i = 0; i < cache.cant_bloques; i++) {
		cache.bloques[i].valido = 0;
	}
	cache.misses = 0;
	cache.accesos = 0;

	//inicializar memoria en 0? podria iterar todas las posiciones, vale la pena? Podria generar bloques en cero mas facil y cargarlos.
} 

//devolver el conjunto de caché al que mapea la dirección address.
unsigned int find_set(int address) {
	return (address >> 1) % cache.cant_sets; //podria sino chequear los bits. address/2 por ser palabras de 2 bytes
}

//devolver el bloque más ’antiguo’ dentro de un conjunto, utilizando el campo correspondiente de los metadatos de los bloques del conjunto.
// --> devuelve el indice del bloque en el arrray de bloques (la posicion en cache).
unsigned int find_earliest(int setnum) {
	return setnum*cache.tam_set + cache.topes[setnum];
}

void load_block_to_cache(bloque_t bloque, int setnum) {
	cache.bloques[find_earliest(setnum)] = bloque;

	if (cache.topes[setnum] >= cache.vias - 1)
		cache.topes[setnum] = 0;
	else 
		cache.topes[setnum]++;
}

//leer el bloque blocknum de memoria y guardarlo en el lugar que le corresponda en la memoria caché
void read_block(int blocknum) {
	unsigned int address = blocknum * tam_bloque;
	bloque_t bloque;

	for (int i = 0; i < cache.tam_bloque; i++) { //cargo el bloque. Caches reales leen en paralelo y aceleran la lectura asi
		bloque.dato[i] = memoria.bloques[address + i]
	}
	bloque.valido = 1;
	bloque.tag = blocknum;
	load_block_to_cache(bloque, find_set(address));
}
// word = unsigned int(int(memoria.bloques[address]) << 8 | memoria.bloques[address]); //leo 2 bytes y los cargo en una word.

//escribir el valor value en la memoria
void write_byte_tomem(int address, char value) {
	memoria.bloques[address] = value;
}

//retornar el valor correspondiente a la posición de memoria address, buscándolo primero en el caché
//escribir 1 en *hit si es un hit y 0 si es un miss
//sólo debe interactuar con la memoria a través de las otras primitivas.
char read_byte(int address, char *hit) {
	*hit = 0;
	if (/*usar tag para ver si cache tiene el bloque correspondiente a la address*/) {
		*hit = 1;
		return cache.bloques[find_set(address) + ];
	}
	return read_block(address);
}

//escribir el valor value en la posición correcta del bloque que corresponde a address, en la memoria y, si está, en el caché
//devolver 1 si es un hit y 0 si es un miss.
//sólo debe interactuar con la memoria a través de las otras primitivas.
char write_byte(int address, char value, char *hit){

}

//devolver el porcentaje de misses desde que se inicializó el cache
char get_miss_rate();

