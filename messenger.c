
#include "messenger.h"

#include "assert.h"
#include "stdlib.h"
#include "string.h"

/* smol_message */

void init_message(smol_message* t_message, unsigned int t_type, unsigned int t_data_size, const void* t_data_ptr) {
	
	assert(t_message);
	void* buffer = malloc(t_data_size);
	const smol_message message = {t_type, buffer, t_data_size, 0};
	
	if (t_data_size) { 
		
		memcpy(buffer, t_data_ptr, t_data_size);
	}
	memcpy(t_message, &message, sizeof(smol_message));
}

smol_message* alloc_message(unsigned int t_type, unsigned int t_data_size, const void* t_data_ptr) {
	
	smol_message* output = malloc(sizeof(smol_message));
	init_message(output, t_type, t_data_size, t_data_ptr);
	return output;
}

void final_message(smol_message* t_message) {
	
	assert(t_message);
	free((void*)t_message->buffer);
}

void free_message(smol_message* t_message) {
	
	assert(t_message);
	final_message(t_message);
	free(t_message);
}

/* smol_message_queue */

smol_message_queue* alloc_message_queue() {
	
	smol_message_queue* output = malloc(sizeof(smol_message_queue));
	
	init_message(&output->tail, 0, 0, 0);
	output->head = &output->tail;
	return output;
}

void free_message_queue(smol_message_queue* t_queue) {
	
	assert(t_queue);
	final_message(&t_queue->tail);
	free(t_queue);
}

int queue_has_message(smol_message_queue* t_queue) {
	
	assert(t_queue);
	
	return t_queue->head != &t_queue->tail;
}

void queue_message(smol_message_queue* t_queue, smol_message* t_message) {
	
	assert(t_queue && t_message && !t_message->prev);
	
	t_queue->head->prev = t_message;
	t_queue->head = t_message;
}

smol_message* pop_message(smol_message_queue* t_queue, int t_should_free) {
	
	assert(t_queue && queue_has_message(t_queue));
	
	smol_message* output = t_queue->tail.prev;
	t_queue->tail.prev = output->prev;
	
	if (t_should_free) {
		
		free_message(output);
	}
	return output;
}