
#include "main.h"

window surface;
graphics context;

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

void update(double t_delta) {
	
	if (t_delta > 0.0) {
		
	}
}

void render() {
	
	RECT rect;
	GetWindowRect(surface.handle, &rect);
	glViewport(rect.left, rect.top, rect.bottom - rect.left, rect.bottom - rect.top);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	graphics_swap_buffers(&context);
}

int main() {
	
	
	SYS("initializing window");
	
	init_window(&surface, window_proc);
	
	SYS("initializing graphics");
	
	init_graphics(&context, &surface);
	
	shader my_shader;
	const char* my_shader_strings[] = 
		{ vs_cont_header
		, vs_cont_vertex_pos_col_norm_uv
		, vs_cont_uniform_wvp
		, vs_cont_body_pos_col_norm_uv_wvp
		};
	const int my_shader_string_lengths[] =
		{ (int)strlen(vs_cont_header)
		, (int)strlen(vs_cont_vertex_pos_col_norm_uv)
		, (int)strlen(vs_cont_uniform_wvp)
		, (int)strlen(vs_cont_body_pos_col_norm_uv_wvp)
		};
	init_shader(&my_shader, GL_VERTEX_SHADER, 4, my_shader_strings, my_shader_string_lengths);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	SYS("entering main loop");
	
	int is_running = 1;
	double delta;
	long long diff, start, length;
	MSG message;
	
	while (is_running) {
		
		diff = clock() - start;
		delta = (double)diff / (double)CLOCKS_PER_SEC;
		
		start = clock();
		
		/* windows message handling */
		
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			
			if (message.message == WM_QUIT || message.message == WM_CLOSE) {
				
				SYS("setting running to false");
				
				is_running = 0;
				
				SYS("exiting main loop");
				
				break;
			}
			
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		/* do stuff */
		
		update(delta);
		render();
		
		/* timing */
		
		diff = clock() - start;
		length = diff * (1000 / CLOCKS_PER_SEC);
		if (length < 1000 / 60) {
			
			Sleep(1000 / 60 - length);
		}
	}
	
	
	SYS("finalizing graphics");
	
	final_graphics(&context);
	
	SYS("finalizing window");
	
	final_window(&surface);
	
	return 0;
}