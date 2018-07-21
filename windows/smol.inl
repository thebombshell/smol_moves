
#ifndef _WIN32

typedef char include_for_windows_only[-1];

#endif

#include <stdlib.h>
#include <time.h>

#include "duktape.h"
#include "graphics.h"
#include "../helper.h"
#include "../log.h"
#include "../logic.h"
#include "../messenger.h"
#include "window.h"

/* windows procedure */

LRESULT CALLBACK window_proc(HWND t_handle, UINT t_message, WPARAM t_wparam, LPARAM t_lparam) {
	
	switch (t_message) {
		
		case WM_CLOSE:
			
			SYS("window received close request");
			
			PostQuitMessage(0); 
		return 0;
		case WM_QUIT:
			
			SYS("window received quit request");
			DestroyWindow(t_handle);
		return 0;
	}
	return DefWindowProc(t_handle, t_message, t_wparam, t_lparam);
}

/* a thread safe message queue */
typedef struct smol_thread_message_queue {
	
	HANDLE write_mutex;
	HANDLE read_mutex;
	HANDLE message_event;
	smol_message_queue* write_queue;
	smol_message_queue* read_queue;
} smol_thread_message_queue;

/* an javascript enabled thread */
typedef struct smol_worker {
	
	duk_context* context;
	smol* smol_context;
	smol_thread_message_queue queue;
	void (*callback)(const smol_message* t_message);
	HANDLE thread;
} smol_worker;

/* the implementation of smol */
typedef struct smol {
	
	window surface;
	graphics context;
	smol_scene scene;
	HANDLE running_mutex;
	int is_running;
	
	smol_worker logic;
	smol_worker helper;
	smol_thread_message_queue queue;
	
} smol;

/* duk */

duk_ret_t js_log(duk_context* t_context) {
	
	assert(t_context);
	
	if (duk_is_string(t_context, 0)) {
		
		LOG("[js]: %s", duk_to_string(t_context, 0));
	}
	else {
		
		LOG("[js]:");
	}
	return 0;
}

duk_ret_t js_wrn(duk_context* t_context) {
	
	assert(t_context);
	
	if (duk_is_string(t_context, 0)) {
		
		WRN("[js]: %s", duk_to_string(t_context, 0));
	}
	else {
		
		WRN("[js]:");
	}
	return 0;
}

duk_ret_t js_err(duk_context* t_context) {
	
	assert(t_context);
	
	if (duk_is_string(t_context, 0)) {
		
		ERR("[js]: %s", duk_to_string(t_context, 0));
	}
	else {
		
		ERR("[js]:");
	}
	return 0;
}

void setup_scripting_environment(duk_context* t_context) {
	
	assert(t_context);
	
	const char* global_impl = "																							\
	var global = new Function(\"return this;\")();																		\
	Object.defineProperty(global, \"global\", { value: global, writable: true, enumerable: false, configurable: true});	";
	
	duk_push_c_function(t_context, js_log, 1);
	duk_put_global_string(t_context, "LOG");
	
	duk_push_c_function(t_context, js_wrn, 1);
	duk_put_global_string(t_context, "WRN");
	
	duk_push_c_function(t_context, js_err, 1);
	duk_put_global_string(t_context, "ERR");
	
	duk_push_string(t_context, global_impl);
	if (duk_peval(t_context)) {
		
		ERR("failed to evaluate global instantiation in script");
	}
	duk_pop(t_context);
	
	duk_eval_string_noresult(t_context, "LOG(\"hello world\"); global.WRN(\"your about to end in \" + 2020); ERR({banana: true, apple: false});");
}

/* smol_thread_message_queue */

void init_threaded_message_queue(smol_thread_message_queue* t_queue) {
	
	SYS("initializing threaded message queue...");
	
	SYS("creating write mutex");
	
	t_queue->write_mutex = CreateMutex(0, 0, 0);
	
	SYS("creating read mutex");
	
	t_queue->read_mutex = CreateMutex(0, 0, 0);
	
	SYS("creating message event");
	
	t_queue->message_event = CreateEvent(0, 0, 0, 0);
	
	SYS("allocating write queue");
	
	t_queue->write_queue = alloc_message_queue();
	
	SYS("allocating read queue");
	
	t_queue->read_queue = alloc_message_queue();
	
	SYS("...threaded message queue initialized");
}

int final_threaded_message_queue(smol_thread_message_queue* t_queue) {
	
	assert(t_queue);
	
	SYS("finalizing threaded message queue...");
	
	SYS("closing write mutex");
	
	if (!CloseHandle(t_queue->write_mutex)) {
		
		ERR("failed to close write mutex");
		return 0;
	}
	
	SYS("closing read mutex");
	
	if (!CloseHandle(t_queue->read_mutex)) {
		
		ERR("failed to close read mutex");
		return 0;
	}
	
	SYS("closing message event");
	
	if (!CloseHandle(t_queue->message_event)) {
		
		ERR("failed to close message event");
		return 0;
	}
	
	SYS("freeing write queue");
	
	free_message_queue(t_queue->write_queue);
	
	SYS("freeing read queue");
	
	free_message_queue(t_queue->read_queue);
	
	SYS("...threaded message queue finalized");
	
	return 1;
}

int post_threaded_message(smol_thread_message_queue* t_queue, unsigned int t_type, unsigned int t_data_size, const void* t_data_ptr) {
	
	assert(t_queue);
	
	smol_message* message;
	DWORD result = WaitForSingleObject(t_queue->write_mutex, INFINITE);
	if (result) {
		
		ERR("failed to receive write mutex for posting a message");
		return 0;
	}
	
	message = alloc_message(t_type, t_data_size, t_data_ptr);
	queue_message(t_queue->write_queue, message);
	
	if (!ReleaseMutex(t_queue->write_mutex)) {
		
		ERR("failed to release write mutex from posting a message");
		return 0;
	}
	SetEvent(t_queue->message_event);
	
	return 1;
}

int handle_threaded_messages(smol_thread_message_queue* t_queue, void (*t_callback)(const smol_message* t_message)) {
	
	assert(t_queue && t_callback);
	
	HANDLE handles[2] = {t_queue->write_mutex, t_queue->read_mutex};
	smol_message* message;
	smol_message_queue* queue;
	
	DWORD result = WaitForMultipleObjects(2, &handles[0], 1, INFINITE);
	if (result) {
		
		ERR("failed to receive access to a read and write mutex for handling threaded messages");
		return 0;
	}
	
	queue = t_queue->write_queue;
	t_queue->write_queue = t_queue->read_queue;
	t_queue->read_queue = queue;
	
	if (!ReleaseMutex(t_queue->write_mutex)) {
		
		ERR("failed to release a write mutex from handling threaded messages");
		return 0;
	}
	
	while (queue_has_message(queue)) {
		
		message = pop_message(queue, 0);
		
		t_callback(message);
		
		free_message(message);
	}
		
	if (!ReleaseMutex(t_queue->read_mutex)) {
		
		ERR("failed to release a read mutex from handling threaded messages");
		return 0;
	}
	return 1;
}

/* smol_worker */

DWORD WINAPI worker_func(void* t_data) {
	
	assert(t_data);
	
	smol_worker* self = (smol_worker*)t_data;
	while (smol_is_running(self->smol_context)) {
		
		WaitForSingleObject(self->queue.message_event, INFINITE);
		if (!handle_threaded_messages(&self->queue, self->callback)) {
			
			ERR("failed to handle worker messages");
			return -1;
		}
	}
	return 0;
}

void init_worker(smol_worker* t_worker, smol* t_smol, void (*t_callback)(const smol_message* t_message)) {
	
	assert(t_worker && t_smol && t_callback);
	
	SYS("initializing worker...");
	
	assert(t_worker);
	
	t_worker->smol_context = t_smol;
	t_worker->callback = t_callback;
	
	SYS("creating scripting context");
	
	t_worker->context = duk_create_heap_default();
	setup_scripting_environment(t_worker->context);
	
	SYS("initializing message queue");
	
	init_threaded_message_queue(&t_worker->queue);
	
	SYS("creating worker thread");
	
	t_worker->thread = CreateThread(0, 0, worker_func, t_worker, 0, 0);
	
	SYS("...worker initialized");
}

int final_worker(smol_worker* t_worker) {
	
	SYS("finalizing worker...");
	
	assert(t_worker);
	
	DWORD result = WaitForSingleObject(t_worker->thread, INFINITE);
	if (result) {
		
		ERR("failed to receive one or more of the threading objects while finalizing a worker");
		return 0;
	}
	
	SYS("closing worker thread");
	
	if (!CloseHandle(t_worker->thread)) {
		
		ERR("failed to close worker thread");
		return 0;
	}
	
	SYS("finalizing message queue");
	
	final_threaded_message_queue(&t_worker->queue);
	
	SYS("destroying scripting context");
	
	duk_destroy_heap(t_worker->context);
	
	SYS("...worker finalized");
	
	return 1;
}

/* smol */

int smol_is_valid(smol* t_smol) {
	
	return t_smol && t_smol;
}

int smol_is_running(smol* t_smol) {
	
	assert(t_smol);
	
	int temp;
	
	if (WaitForSingleObject(t_smol->running_mutex, INFINITE)) {
		
		ERR("failed to receive running mutex to query smol state");
	}
	
	temp = t_smol->is_running;
	
	if (!ReleaseMutex(t_smol->running_mutex)) {
		
		ERR("failed to release running mutex from smol state query");
	}
	
	return temp;
}

void smol_set_running(smol* t_smol, int t_value) {
	
	assert(t_smol);
	
	if (WaitForSingleObject(t_smol->running_mutex, INFINITE)) {
		
		ERR("failed to receive running mutex to set smol state");
	}
	
	t_smol->is_running = t_value;
	
	if (!ReleaseMutex(t_smol->running_mutex)) {
		
		ERR("failed to release running mutex from setting smol state");
	}
}

smol* alloc_smol() {
	
	SYS("allocating smol context for windows...");
	
	smol* output = malloc(sizeof(smol));
	
	output->is_running = 0;
	
	SYS("creating running mutex");
	
	output->running_mutex = CreateMutex(0, 0, 0);
	
	SYS("initializing window");
	
	init_window(&output->surface, window_proc);
	
	SYS("initializing graphics");
	
	init_graphics(&output->context, &output->surface);
	
	SYS("initializing rendering scene");
	
	smol_init_scene(&output->scene);
	
	SYS("initializing message queue");
	
	init_threaded_message_queue(&output->queue);
	
	SYS("initializing logic worker");
	
	init_worker(&output->logic, output, logic_callback);
	
	SYS("initializing helper worker");
	
	init_worker(&output->helper, output, helper_callback);
	
	SYS("...smol context allocated");
	
	return output;
}

void free_smol(smol* t_smol) {
	
	SYS("freeing smol context for windows...");
	
	smol_set_running(t_smol, 0);
	
	SYS("finalizing helper worker");
	
	if (!final_worker(&t_smol->helper)) {
		
		ERR("failed to finalize helper worker");
	}
	
	SYS("finalizing logic worker");
	
	if (!final_worker(&t_smol->logic)) {
		
		ERR("failed to finalize logic worker");
	}
	
	SYS("finalizing render scene");
	
	smol_final_scene(&t_smol->scene);
	
	SYS("finalizing message queue");
	
	final_threaded_message_queue(&t_smol->queue);
	
	SYS("finalizing graphics");
	
	final_graphics(&t_smol->context);
	
	SYS("finalizing window");
	
	final_window(&t_smol->surface);
	
	SYS("freeing context");
	
	free(t_smol);
	
	SYS("...smol context freed");
}

void smol_callback(const smol_message* t_message) {
	
	assert(t_message);
	
	
}

void smol_run(smol* t_smol) {
	
	assert(t_smol);
	
	clock_t start, diff;
	unsigned long length;
	unsigned long frame_counter = 0;
	double frame_timer = 0.0;
	double delta;
	MSG message;
	
	SYS("running smol context...");
	
	SYS("setting running to true");
	
	smol_set_running(t_smol, 1);
	
	SYS("entering main loop");
	
	while (smol_is_running(t_smol)) {
		
		diff = clock() - start;
		delta = (double)diff / (double)CLOCKS_PER_SEC;
		
		start = clock();
		
		/* fps counting and printing*/
		
		frame_timer += delta;
		if (frame_timer > 1.0) {
			
			SYS("fps: %lu", frame_counter);
			frame_timer = 0.0;
			frame_counter = 0;
		}
		++frame_counter;
		
		/* windows message handling */
		
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			
			if (message.message == WM_QUIT || message.message == WM_CLOSE) {
				
				SYS("setting running to false");
				
				smol_set_running(t_smol, 0);
				
				SYS("exiting main loop");
				
				break;
			}
			
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		/* thread message handling */
		
		handle_threaded_messages(&t_smol->queue, smol_callback);
		
		/* render */
		
		smol_render(t_smol);
		
		/* timing */
		
		diff = clock() - start;
		length = diff * (1000 / CLOCKS_PER_SEC);
		if (length < 1000 / 60) {
			
			Sleep(1000 / 60 - length);
		}
	}
	
	SYS("...smol context is stopped");
}

smol_scene* smol_get_scene(smol* t_smol) {
	
	assert(t_smol);
	
	return &t_smol->scene;
}


void smol_swap_buffers(smol* t_smol) {
	
	assert(t_smol);
	
	graphics_swap_buffers(&t_smol->context);
}
