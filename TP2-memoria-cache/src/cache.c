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

unsigned int find_set(int address) {
	return (address/cache.tam_bloque) % cache.cant_sets;
}

unsigned int find_earliest(int setnum) {
	return setnum * cache.vias + cache.topes[setnum];
}

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

void write_byte_tomem(int address, char value) {
	unsigned int block_offset = address & ~(0xffff / cache.cant_bloques); // si la cant de bloques no es potencia de 2 pasan cosas raras, es mejor usar >> log2(cant_bloques), o ver que pasa usando %, como find_set
	memoria.bloques[address / cache.tam_bloque].datos[block_offset] = value;
}

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

char get_miss_rate() {
	return cache.misses / cache.accesos;
}