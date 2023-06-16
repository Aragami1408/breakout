#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include <assert.h>

#include "utils.h"
#include "math.c"
#include "platform_common.c"

#define FPS 60

typedef struct {
	// Platform non-specific part
	int width, height;
	u32 *pixels;
	// Platform specific part
	BITMAPINFO bitmap_info;
} RenderBuffer;

global_variable bool running = true;

global_variable RenderBuffer render_buffer;
global_variable bool character = false;
global_variable Input input;

#include "software_rendering.c"
#include "game.c"


LRESULT CALLBACK window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	LRESULT result = 0;
	switch(message) {
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;
		
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_KEYDOWN: {
			
			u32 vk_code = (u32)wparam;
			bool was_down = ((lparam & (1 << 30)) != 0);
			bool is_down = ((lparam & (1 << 31)) == 0);
			
#define process_button(vk, b) \
if (vk_code == (vk)) { \
input.buttons[(b)].changed = is_down != input.buttons[(b)].is_down; \
input.buttons[(b)].is_down = is_down; \
}
			
			process_button(VK_LEFT, BUTTON_LEFT);
			process_button(VK_RIGHT, BUTTON_RIGHT);
			process_button(VK_UP, BUTTON_UP);
			process_button(VK_DOWN, BUTTON_DOWN);
		} break;
		
		case WM_SIZE: {
			RECT rect;
			GetWindowRect(window, &rect);
			
			render_buffer.width = rect.right - rect.left;
			render_buffer.height = rect.bottom - rect.top;
			
			int buffer_pixels_size = render_buffer.width * render_buffer.height * sizeof(u32);
			
			if(render_buffer.pixels) {
				VirtualFree(render_buffer.pixels, 0, MEM_RELEASE);
			}
			
			render_buffer.pixels = VirtualAlloc(0, buffer_pixels_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			
			render_buffer.bitmap_info.bmiHeader.biSize = sizeof(render_buffer.bitmap_info.bmiHeader);
			render_buffer.bitmap_info.bmiHeader.biWidth = render_buffer.width;
			render_buffer.bitmap_info.bmiHeader.biHeight = render_buffer.height;
			render_buffer.bitmap_info.bmiHeader.biPlanes = 1;
			render_buffer.bitmap_info.bmiHeader.biBitCount = 32;
			render_buffer.bitmap_info.bmiHeader.biCompression = BI_RGB;
			
			
		} break;
		
		default: {
			result = DefWindowProcA(window, message, wparam, lparam);
		} break;
	}
	return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	WNDCLASSA window_class = {0};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = window_callback;
	
	window_class.lpszClassName = "Game_Window_Class";
	
	RegisterClassA(&window_class);
	
	HWND window = CreateWindowExA(0, window_class.lpszClassName, "Break Arcade Games Out", WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0);
	HDC hdc = GetDC(window);
	
	LARGE_INTEGER last_counter;
	QueryPerformanceCounter(&last_counter);
	
	LARGE_INTEGER frequency_counter_large;
	QueryPerformanceFrequency(&frequency_counter_large);
	f32 frequency_counter = (f32)frequency_counter_large.QuadPart;
	
	f32 last_dt = 1 / FPS;
	
	while(running) {
		// Input
		for(int i = 0; i < BUTTON_COUNT; i++) input.buttons[i].changed = false;
		
		MSG message;
		while(PeekMessageA(&message, window, 0, 0, PM_REMOVE) != 0) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		POINT mouse_pointer;
		GetCursorPos(&mouse_pointer);
		ScreenToClient(window, &mouse_pointer);
		
		input.mouse.x = mouse_pointer.x;
		input.mouse.y = render_buffer.height - mouse_pointer.y;
		
		// Simulate
		simulate_game(&input, last_dt);
		
		// Render
		StretchDIBits(hdc, 0, 0, render_buffer.width, render_buffer.height, 0, 0, render_buffer.width, render_buffer.height, render_buffer.pixels, &render_buffer.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		
		// Get the frame time
		LARGE_INTEGER current_counter;
		QueryPerformanceCounter(&current_counter);
		
		last_dt = (f32) (current_counter.QuadPart - last_counter.QuadPart) / frequency_counter;
		
		last_counter = current_counter;
	}
	
	return 0;
}
