
/** @file		messenger.h
 *	@brief		The cross thread messenger of the smol framework
 *	@details	
 *	@author		[Scott R Howell (Bombshell)](http://scottrhowell.com/) [\@GIT](https://github.com/thebombshell)
 *	@date		15th July 2018
 */

#ifndef MESSENGER_H
#define MESSENGER_H

typedef struct smol_message smol_message;

typedef struct smol_message {
	
	const unsigned int type;
	const void* buffer;
	const unsigned int buffer_size;
	smol_message* prev;
} smol_message;

smol_message* alloc_message(unsigned int t_type, unsigned int t_data_size, const void* t_data_ptr);

void free_message(smol_message* t_message);

typedef struct smol_message_queue {
	
	smol_message* head;
	smol_message tail;
} smol_message_queue;

smol_message_queue* alloc_message_queue();

void free_message_queue(smol_message_queue* t_queue);

int queue_has_message(smol_message_queue* t_queue);

void queue_message(smol_message_queue* t_queue, smol_message* t_message);

smol_message* pop_message(smol_message_queue* t_queue, int t_should_free);

#endif