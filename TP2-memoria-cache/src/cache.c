//cache asociativa por conjuntos
//cantidad de vias, capacidad y tamaño de bloque variables
//politica de reemplazo FIFO y escritura WT/¬WA
//espacio de direcciones de 16 bits
//cada bloque de cache cuenta con su metadata, incluyendo bit V y tag, y campo que permita implementar la politica FIFO.


typedef struct memoria_16_bits {
	const unsigned int CAPACIDAD = 0xffff // 65535 bytes
	bloque_t bloques[CAPACIDAD/tam_bloque];
} memoria_t;

typedef struct cache {
	unsigned short int vias;
	unsigned short int capacidad;
	unsigned short int tam_bloque;
	
	unsigned short int tam_set = tam_bloque * vias;
	unsigned short int cant_sets = capacidad / (tam_bloque * vias);
	unsigned short int cant_bloques =  capacidad / tam_bloque;

	bloque_t bloques[cant_bloques];
	unsigned short int topes[cant_sets];
	unsigned short int accesos;
	unsigned short int misses;
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
	return (address/cache.tam_bloque) % cache.cant_sets;
}

//devolver el bloque más ’antiguo’ dentro de un conjunto, utilizando el campo correspondiente de los metadatos de los bloques del conjunto.
// --> devuelve el indice del bloque mas antiguo en el array de bloques (la posicion en cache).
unsigned int find_earliest(int setnum) {
	return setnum * cache.vias + cache.topes[setnum];
}

//leer el bloque blocknum de memoria y guardarlo en el lugar que le corresponda en la memoria caché
void read_block(int blocknum) {
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
// devuelve el indice del bloque en el array de bloques (la posicion en cache).
int find_block(int setnum, char* hit){
	short int i = 0, blocknum;
	*hit = 0;
	while (!(*hit) && i < cache.vias) {
		blocknum = setnum * vias + i;
		if (cache.bloques[blocknum].tag = setnum) {
			*hit = 1;
		}
	}
	return blocknum;
}

//retornar el valor correspondiente a la posición de memoria address, buscándolo primero en el caché
//escribir 1 en *hit si es un hit y 0 si es un miss
//sólo debe interactuar con la memoria a través de las otras primitivas.
char read_byte(int address, char *hit) {
	find_block(find_set(address));
	
	if *hit == 0
		read_block(address / cache.tam_bloque);

	unsigned short int mascara = (0xffff / cache.tam_bloque) & ~(0xffff / cache.cant_sets);
	unsigned short int set_offset = address & mascara;
	unsigned short int setnum = find_set(address) * vias;
	return cache.bloques[setnum + set_offset];
}

//escribir el valor value en la posición correcta del bloque que corresponde a address, en la memoria y, si está, en el caché
//devolver 1 si es un hit y 0 si es un miss.
//sólo debe interactuar con la memoria a través de las otras primitivas.
char write_byte(int address, char value, char *hit){
	int blocknum = find_block(find_set(address));

	if (*hit == 1) {
		unsigned short int mascara = (0xffff / cache.tam_bloque) & ~(0xffff / cache.cant_sets);
		unsigned short int set_offset = address & mascara;
		unsigned short int setnum = find_set(address) * vias;
		unsigned int block_offset = address & ~(0xffff / cache.cant_bloques); // si la cant de bloques no es potencia de 2 pasan cosas raras, es mejor usar >> log2(cant_bloques), o ver que pasa usando %, como find_set
		cache.bloques[setnum + set_offset].datos[block_offset] = value;
	}
	write_byte_tomem(address, value);
}

//devolver el porcentaje de misses desde que se inicializó el cache
char get_miss_rate() {
	cache.misses / (cache.accesos);
}

