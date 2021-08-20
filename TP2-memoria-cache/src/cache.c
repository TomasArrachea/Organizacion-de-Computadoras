#include "cache.h"
#include <stdlib.h>

cache_t cache;
memoria_t memoria;

bloque_t new_bloque(int tamanio) {
	bloque_t bloque;
	bloque.valido = 0;
	bloque.tag = 0;
	bloque.datos = malloc(tamanio * sizeof(char));
	return bloque;
}

void init_memoria(short unsigned int tam_bloque) {
	memoria.capacidad = 0xffff;
	memoria.tam_bloque = tam_bloque;

	unsigned short int cant_bloques_memoria = memoria.capacidad / memoria.tam_bloque;
	memoria.bloques = malloc(cant_bloques_memoria * sizeof(bloque_t));

	for (int i = 0; i < cant_bloques_memoria; i++) {
		memoria.bloques[i] = new_bloque(memoria.tam_bloque);
	}
}

void init_cache(short unsigned int vias, short unsigned int capacidad) {
	cache.vias = vias;
	cache.capacidad = capacidad;
	cache.misses = 0;
	cache.accesos = 0;

	unsigned short int cant_bloques = cache.capacidad / memoria.tam_bloque;
	cache.bloques = malloc(cant_bloques * sizeof(bloque_t));
	for (int i = 0; i < cant_bloques; i++) {
		cache.bloques[i] = new_bloque(memoria.tam_bloque);
	}

	unsigned short int cant_sets = capacidad / (memoria.tam_bloque * vias);
	cache.topes = malloc(cant_sets * sizeof(short int));
	for (int i = 0; i < cant_sets; i++) {
		cache.topes[i] = 0;
	}
}

void init() {
	unsigned short int cant_bloques_cache = cache.capacidad / memoria.tam_bloque;
	for (int i = 0; i < cant_bloques_cache; i++) {
		cache.bloques[i].valido = 0;
	}
	cache.misses = 0;
	cache.accesos = 0;

	unsigned short int cant_bloques_memoria = memoria.capacidad / memoria.tam_bloque;
	for (int i = 0; i < cant_bloques_memoria; i++) {
		memoria.bloques[i].valido = 0;
		for (int j = 0; j < memoria.tam_bloque; j++) {
			memoria.bloques[i].datos[j] = 0;
		}
	}
}

void delete_memoria() {
	unsigned short int cant_bloques_memoria = memoria.capacidad / memoria.tam_bloque;
	for (int i = 0; i < cant_bloques_memoria; i++) {
		free(memoria.bloques[i].datos);
	}
	free(memoria.bloques);
}

void delete_cache() {
	free(cache.bloques);
	free(cache.topes);
}

unsigned int find_set(int address) {
	unsigned short int cant_sets = cache.capacidad / (memoria.tam_bloque * cache.vias);
	return (address/memoria.tam_bloque) % cant_sets;
}

unsigned int find_earliest(int setnum) {
	return setnum*cache.vias + cache.topes[setnum];
}

void read_block(int blocknum) {
	unsigned short int setnum = find_set(blocknum * memoria.tam_bloque);
	unsigned short int pos = find_earliest(setnum);

	cache.bloques[pos] = memoria.bloques[blocknum]; 
	cache.bloques[pos].valido = 1;
	cache.bloques[pos].tag = blocknum / cache.vias;

	if (cache.topes[setnum] >= cache.vias - 1)
		cache.topes[setnum] = 0;
	else 
		cache.topes[setnum]++;
}

void write_byte_tomem(int address, char value) {
	unsigned short int block_offset = address % memoria.tam_bloque;
	memoria.bloques[address / memoria.tam_bloque].datos[block_offset] = value;
}

unsigned short int find_block(int address, char* hit){
	unsigned short int tag = address / (cache.vias*memoria.tam_bloque);
	unsigned short int setnum = find_set(address);
	unsigned short int i = 0, pos;
	bloque_t bloque;
	*hit = 0;

	while (!(*hit) && i < cache.vias) {
		pos = setnum*cache.vias + i;
		bloque = cache.bloques[pos];
		if (bloque.tag == tag && bloque.valido) {
			*hit = 1;
		}
		i++;
	}
	return pos;
}

char read_byte(int address, char *hit) {
	if (address >= memoria.capacidad || address < 0) { // si no existe la direccion en memoria, SEGFAULT
		*hit = -1;
		return 0;
	}

	unsigned short int pos = find_block(address, hit);
	if (*hit == 0) {
		read_block(address / memoria.tam_bloque);
		pos = find_block(address, hit);
		*hit = 0;
		cache.misses++;
	}
	cache.accesos++;
	unsigned int block_offset = address % memoria.tam_bloque;
	return cache.bloques[pos].datos[block_offset];
}

char write_byte(int address, char value){
	if (address >= memoria.capacidad || address < 0) // si no existe la direccion en memoria, SEGFAULT
		return -1;

	char hit;
	unsigned short int pos = find_block(address, &hit);
	cache.accesos++;

	if (hit == 1) {
		unsigned int block_offset = address % memoria.tam_bloque;
		cache.bloques[pos].datos[block_offset] = value;
	} else
		cache.misses++;

	write_byte_tomem(address, value);
	return hit;
}

char get_miss_rate() {
	return (cache.misses*100 / cache.accesos);
}