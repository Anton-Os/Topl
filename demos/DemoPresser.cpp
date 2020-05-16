
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Input.hpp"

#include <cstdio>

void callback1(void){
	puts("Key k Pressed!");
}

KeyState kState('k', KEY_release);
// Lowercase k becomes uppercase 

char kc = '\0';
enum KEY_Event event = KEY_none;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	static Input_KeyLogger keyLogger;
	if (keyLogger.getCallbackCount() == 0) {
		keyLogger.addCallback(&kState, callback1);
	}

	switch (message) {
	case (WM_CREATE): {
	}
	case (WM_PAINT): {
	}
	case(WM_KEYDOWN): {
		event = KEY_press;
		puts("Key pressed \n");
	}
	case(WM_KEYUP): {
		event = KEY_release;
		puts("Key released");
	}
	case (WM_CHAR): {
		char ch = (char)wParam;
		putchar(ch);
		putchar('\n');

		kc = ch;
		if (kc != '\0' && event != KEY_none) {
			keyLogger.addKeyEvent(kc, event);
			kc = '\0'; event = KEY_none;
		}
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

int main(int argc, char** argv) {

	WNDCLASS wndClass = { 0 };
	// wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = "Default Class";
	RegisterClass(&wndClass);

	HWND wndWindow = CreateWindow(
		"Default Class",
		"Team Purple",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1100,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(wndWindow, 1);
	UpdateWindow(wndWindow);

	MSG wndMessage;
	BOOL bRet;

    while((bRet = GetMessage(&wndMessage, (HWND)NULL, 0, 0)) != 0){
        if(bRet == -1) return -1;
        // else if(TranslateAccelerator(wndWindow, ))
        else {
            TranslateMessage(&wndMessage);
            DispatchMessage(&wndMessage);
        }
    }
    

	return 0;
}