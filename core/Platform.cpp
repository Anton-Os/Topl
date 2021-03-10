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
    mContext.display = XOpenDisplay(NULL);

	GLint visualInfoAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo* visualInfo = glXChooseVisual(mContext.display, 0, visualInfoAttribs);

	XSetWindowAttributes windowAttribs;
	windowAttribs.colormap = XCreateColormap(mContext.display, DefaultRootWindow(mContext.display), visualInfo->visual, AllocNone);
	windowAttribs.event_mask = ExposureMask | KeyPressMask;

    return XCreateWindow(
		mContext.display, DefaultRootWindow(mContext.display),
        0, 0,
        TOPL_WIN_WIDTH , TOPL_WIN_HEIGHT,
        0, visualInfo->depth,
        InputOutput, visualInfo->visual,
        CWColormap | CWEventMask,
        &windowAttribs
	);
}

void Platform::setupMainWindow(NATIVE_WINDOW window){
    mContext.window = &window_ptr;

	XSelectInput(mContext.display, window, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask);
	XMapWindow(mContext.display, window);
    return;
}

void Platform::handleEvents(){
    int eventsPending = XEventsQueued(mContext.display, QueuedAferReading);
	XEvent currentEvent;

	while(eventsPending-- > 0){ // Deplete number of events
		XNextEvent(mContext.display, &currentEvent);

		// TODO: Perform event processing logic here
	}
}

#endif