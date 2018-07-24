
/** @file		messenger.h
 *	@brief		The cross thread messenger of the smol framework
 *	@details	
 *	@author		[Scott R Howell (Bombshell)](http://scottrhowell.com/) [\@GIT](https://github.com/thebombshell)
 *	@date		15th July 2018
 */

#ifndef MESSENGER_H
#define MESSENGER_H

#define SMOL_MSG_TARGET_MASK		0x0000000F
#define SMOL_MSG_INFO_MASK			0x000000F0
#define SMOL_MSG_TYPE_MASK			0xFFFFFF00

#define SMOL_MSG_TARGET_SYSTEM		0x00000001
#define SMOL_MSG_TARGET_LOGIC		0x00000002
#define SMOL_MSG_TARGET_HELPER		0x00000004

/**	Message will be disposed of once used
 */
#define SMOL_MSG_INFO_DISPOSE		0x00000000
/**	Message will be reused each message queue until sent again in a dispose message
 */
#define SMOL_MSG_INFO_PERSIST		0x00000010

/**	Dispose of a persistent message
 */
#define SMOL_MSG_DISPOSE			0x00000000
#define SMOL_MSG_DRAW				0x00000100
#define SMOL_MSG_MATERIAL			0x00000200
#define SMOL_MSG_MESH				0x00000300
#define SMOL_MSG_PASS				0x00000400
#define SMOL_MSG_PATH				0x00000500
#define SMOL_MSG_STAGE				0x00000600
#define SMOL_MSG_UNIFORM			0x00000700
#define SMOL_MSG_VIEW				0x00000800

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
	
	smol_message* current;
	smol_message* head;
	smol_message tail;
} smol_message_queue;

smol_message_queue* alloc_message_queue();

void free_message_queue(smol_message_queue* t_queue);

int queue_has_message(smol_message_queue* t_queue);

const smol_message* queue_message(smol_message_queue* t_queue, unsigned int t_type, unsigned int t_data_size, const void* t_data_ptr);

int pop_message(smol_message_queue* t_queue, smol_message* t_message);

void queue_flush(smol_message_queue* t_queue);

#endif