
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct hash_table {
	
	const unsigned int size;
	void*** table;
} hash_table;

void init_hash_table(hash_table* t_hash_table, unsigned int t_hash_size);

void final_hash_table(hash_table* t_hash_table);

int hash_table_insert(hash_table* t_hash_table, const char* t_key, void* t_value);

int hash_table_remove(hash_table* t_hash_table, const char* t_key);

void* hash_table_get(hash_table* t_hash_table, const char* t_key);

#endif