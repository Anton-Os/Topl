#include "Platform.hpp"

namespace Topl {
    Input_KeyLogger keyLogger; // TODO: Integrate as part of Platform
}

#ifdef _WIN32

/********* REMOVE AFTER TESTING ***********/
/* void callback_w(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(0.0f, MOVE_AMOUNT, 0.0f)); } // Move up
void callback_a(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(-1 * MOVE_AMOUNT, 0.0f, 0.0f)); } // Move left
void callback_s(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(0.0f, -1 * MOVE_AMOUNT, 0.0f)); } // Move down
void callback_d(void) { Topl::humanoid.move(&Topl::sceneManager, Eigen::Vector3f(MOVE_AMOUNT, 0.0f, 0.0f)); } // Move right
void callback_r(void) { Topl::humanoid.rotate(&Topl::sceneManager, Eigen::Vector3f(1.0f, 0.0f, 0.0f)); } // Rotate */
/********* REMOVE AFTER TESTING ***********/

LRESULT CALLBACK eventProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

    /********* REMOVE AFTER TESTING ***********/
    /* if (Topl::keyLogger.getCallbackCount() == 0) { // Initialization required
		Topl::keyLogger.addCallback('w', callback_w);
		Topl::keyLogger.addCallback('a', callback_a);
		Topl::keyLogger.addCallback('s', callback_s);
		Topl::keyLogger.addCallback('d', callback_d);
		Topl::keyLogger.addCallback('r', callback_r);
	} */
    /********* REMOVE AFTER TESTING ***********/

	switch (message) {
	case (WM_CREATE): {}
	case (WM_PAINT): {}
	case(WM_KEYDOWN): {}
	case(WM_KEYUP): {}
	case (WM_CHAR): {
		Topl::keyLogger.addKeyPress((char)wParam);
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

NATIVE_WINDOW Platform::createWindow(const char* windowName){
    mContext.windowClass = { 0 };
	mContext.windowClass.hInstance = GetModuleHandle(NULL);
	mContext.windowClass.lpfnWndProc = eventProcedure;
	mContext.windowClass.lpszClassName = "Topl";
	RegisterClass(&mContext.windowClass);

    /* nameSize = strlen(windowName) + 1;
	wchar_t* name_wchar = new wchar_t[nameSize];
	mbstowcs(, windowName, nameSize); // need proper conversion to wchar_t 
    delete name_wchar; */

    return CreateWindow(
        "Topl",
		windowName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 
        TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT,
		NULL, NULL, GetModuleHandle(NULL), NULL
    );
}

void Platform::setupMainWindow(NATIVE_WINDOW window){
    mContext.window_ptr = &window;

    ShowWindow(window, 1);
	UpdateWindow(window);
}

void Platform::handleEvents(){
    while (PeekMessage(&mContext.eventMsg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&mContext.eventMsg);
		DispatchMessage(&mContext.eventMsg);
	}
	if (mContext.eventMsg.message == WM_QUIT) return; // Error code?
}

#elif defined(__linux__)

NATIVE_WINDOW Platform::createWindow(const char* windowName){
    // TODO: Implement window creation code
    return;
}

void Platform::setupMainWindow(NATIVE_WINDOW window){
    // TODO: Implement window showing code
    return;
}

void Platform::handleEvents(){
    // TODO: Implement event handling code
    return;
}

#endif