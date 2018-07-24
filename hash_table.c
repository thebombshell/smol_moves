
#include "hash_table.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void init_hash_table(hash_table* t_hash_table, unsigned int t_hash_size) {
	
	assert(t_hash_table && t_hash_size > 1);
	
	void*** table = calloc(sizeof(void**), t_hash_size);
	const hash_table copy = {t_hash_size, table};
	memcpy(t_hash_table, &copy, sizeof(hash_table));
}

void final_hash_table(hash_table* t_hash_table) {
	
	assert(t_hash_table);
	
	unsigned int i;
	for (i = 0; i < t_hash_table->size; ++i) {
		
		void** iter = t_hash_table->table[i];
		void** prev;
		while (iter) {
			
			prev = iter;
			iter = iter[0];
			free(prev[1]);
			free(prev);
		}
	}
	free(t_hash_table->table);
}

unsigned int hash_string(hash_table* t_hash_table, const char* t_key) {
	
	assert(t_hash_table && t_key);
	
	unsigned int output;
	for (output = 0; *t_key != '\0'; ++t_key) {
		
		output = *t_key + 31 * output;
	}
	return output % t_hash_table->size;
}

int hash_table_insert(hash_table* t_hash_table, const char* t_key, void* t_value) {
	
	assert(t_hash_table && t_key);
	
	unsigned int hash = hash_string(t_hash_table, t_key);
	void** iter = t_hash_table->table[hash];
	
	while (iter) {
		if (strcmp(iter[1], t_key)) {
			
			return 0;
		}
		iter = iter[0];
	}
	iter = iter[0] = malloc(sizeof(void*) * 3);
	iter[0] = 0;
	iter[1] = malloc(sizeof(char) * strlen(t_key));
	strcpy(iter[1], t_key);
	iter[2] = t_value;
	return 1;
}

int hash_table_remove(hash_table* t_hash_table, const char* t_key) {
	
	assert(t_hash_table && t_key);
	
	unsigned int hash = hash_string(t_hash_table, t_key);
	void** iter = t_hash_table->table[hash];
	void** prev;
	int compare = 0;
	compare = strcmp(iter[1], t_key);
	while (iter && compare) {
		
		prev = iter;
		iter = iter[0];
		compare = strcmp(iter[1], t_key);
	}
	if (!compare) {
		
		prev[0] = iter[0];
		free(iter[1]);
		free(iter);
		return 1;
	}
	
	return 0;
}

void* hash_table_get(hash_table* t_hash_table, const char* t_key) {
	
	assert(t_hash_table && t_key);
	
	unsigned int hash = hash_string(t_hash_table, t_key);
	void** iter =t_hash_table->table[hash];
	int compare = 0;
	compare = strcmp(iter[1], t_key);
	while (iter && compare) {
		
		iter = iter[0];
		compare = strcmp(iter[1], t_key);
	}
	if (!compare) {
		
		return iter[2];
	}
	
	return 0;
}