
#include "window.h"

#include "../log.h"

#include <assert.h>

int window_is_valid(window* t_window) {

	return t_window && IsWindow(t_window->handle);
}

int init_window(window* t_window, WNDPROC t_proc) {
	
	assert(t_window);
	
	WNDCLASSEX window_class;
	window_class.cbSize			= sizeof(WNDCLASSEX);
	window_class.style 			= CS_OWNDC;
	window_class.lpfnWndProc 	= t_proc;
	window_class.cbClsExtra 	= 0;
	window_class.cbWndExtra 	= 0;
	window_class.hInstance		= GetModuleHandle(0);
	window_class.hIcon          = LoadIcon(0, IDI_APPLICATION);  
	window_class.hCursor        = LoadCursor(0, IDC_ARROW);  
	window_class.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);  
	window_class.lpszMenuName   = 0;  
	window_class.lpszClassName  = "smol_window";  
	window_class.hIconSm        = LoadIcon(0, IDI_APPLICATION);
	
	RegisterClassEx(&window_class);
	
	t_window->handle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		"smol_window",
		"smol",
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_THICKFRAME,
		100,
		100,
		1280,
		720, 0, 0, window_class.hInstance, 0);
	
	
	if (!t_window->handle) {
		
		ERR("failed to create window");
		return 0;
	}
	ShowWindow(t_window->handle, SW_SHOW);
	return 1;
}

void final_window(window* t_window) {
	
	assert(window_is_valid(t_window));
	
	DestroyWindow(t_window->handle);
}
