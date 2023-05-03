#include "pl0.h"

Hashmap *new_hashmap(int size) {
	Hashmap *map = calloc(1, sizeof(Hashmap));
	map->data = calloc(size, sizeof(Pair*));
	map->size = size;
	return map;
}

Pair *new_pair(char *key, pVal *val) {
	Pair *p = calloc(1, sizeof(Pair));
	p->key = key;
	p->val = val;
	return p;
}

static int make_hash(char *str, int hashmax) {
	int len = strlen(str);
	int hash = 0;
	for (int n = 0, weight = 0; n < len; n++, weight++) {
		if (weight > 7) weight = 0;
		hash += (int)str[n] << (4 * weight);
	}
	return hash % hashmax;
}

static int rehash(Hashmap *map, int firsthash) {
	int hashval;
	for (int i = 1; i <= map->size / 2; i++) {
		hashval = (firsthash + i * i) % map->size;
		if (map->data[hashval] == NULL) return hashval;
	}
	return -1;
}

void add2map(Hashmap *map, Pair *pair) {
	int hashval = make_hash(pair->key, map->size);
	if (map->data[hashval] != NULL) {
		hashval = rehash(map, hashval);
		if (hashval == -1) exit(1);
	}
	map->data[hashval] = pair;
	return;
}

pVal *get_from_map(Hashmap *map, char *key) {
	int hashval = make_hash(key, map->size);
	Pair *pair;
	for (int i = 0; i <= map->size / 2; i++) {
		pair = map->data[(hashval + i * i) % map->size];
		if (pair != NULL && strcmp(key, pair->key) == 0)
			return pair->val;
	}
	exit(1);
}

bool haskey(Hashmap *map, char *key) {
	int hashval = make_hash(key, map->size);
	Pair *pair;
	for (int i = 0; i <= map->size / 2; i++) {
		pair = map->data[(hashval + i * i) % map->size];
		if (pair != NULL && strcmp(key, pair->key) == 0)
			return true;
	}
	return false;
}
