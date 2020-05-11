
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Input.hpp"

#include <cstdio>

void k1_Press(){  puts("Key 2 Pressed!");  }
void k1_Release(){  puts("Key 2 Released!");  }
void k2_Press(){  puts("Key 2 Pressed!");  }
void k2_Release(){  puts("Key 2 Released!");  }

void callback1(const std::vector<KeyState>& states){
	if(states.size() != 2){
		puts("Incorrect number of inputs");
		return;
	}

	const KeyState(const*) k1 = &states[0];
	if(k1->event == KEY_press) k1_Press();
	else if(k1->event == KEY_release) k1_Release();

	const KeyState(const*) k2 = &states[1];
	if(k2->event == KEY_press) k2_Press();
	else if(k2->event == KEY_release) k2_Release();
}

char kc = '\0';
enum KEY_Event event = KEY_none;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

    static Input_KeyLogger keyLogger;
	if(keyLogger.getCallbackCount() == 0){
		// Initialize all the callback stuff
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
        //keyLogger.addKeyEvent(&ch, )
    }
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

    if(kc != '\0' && event != KEY_none){
        keyLogger.addKeyEvent(&kc, event);

        kc = '\0'; // Resetting default key state
        event = KEY_none; // Resetting default key event
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