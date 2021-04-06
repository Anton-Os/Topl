#include "Platform.hpp"

Input_KeyLogger Platform::keyLogger = Input_KeyLogger(); // explicit definition of static member class
Input_MouseLogger Platform::mouseLogger = Input_MouseLogger(); // explicit definition of static member class

#ifdef _WIN32

LRESULT CALLBACK eventProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {}
	case (WM_PAINT): {}
	case(WM_KEYDOWN): {}
	case(WM_KEYUP): {}
	case(WM_MOUSEMOVE):{}
	case (WM_CHAR): { Platform::keyLogger.addKeyPress((char)wParam); }
	case (WM_LBUTTONDOWN): { Platform::mouseLogger.addMousePress(MOUSE_LeftBtn_Down); }
	case (WM_RBUTTONDOWN): { Platform::mouseLogger.addMousePress(MOUSE_RightBtn_Down); }
	case (WM_MBUTTONDOWN): { Platform::mouseLogger.addMousePress(MOUSE_MiddleBtn_Down); }
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

bool Platform::getCursorCoords(float* xPos, float* yPos) { // Optimize this function
	GetCursorPos(&mContext.cursorPos);

	RECT windowRect;
	GetWindowRect(*(mContext.window_ptr), &windowRect);
	
	if (mContext.cursorPos.x > windowRect.right || mContext.cursorPos.x < windowRect.left || mContext.cursorPos.y > windowRect.top || mContext.cursorPos.y < windowRect.bottom)
		return false; // Cursor is outside the screen space

	LONG centerX = windowRect.left + ((windowRect.right - windowRect.left) / 2);
	LONG centerY = windowRect.bottom + ((windowRect.top - windowRect.bottom) / 2);

	// TODO: Continue to Implement

	return true;
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
    mContext.window_ptr = &window;

	XSelectInput(mContext.display, window, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask);
	XMapWindow(mContext.display, window);
    return;
}

void Platform::handleEvents(){
    int eventsPending = XEventsQueued(mContext.display, QueuedAfterReading);
	XEvent currentEvent;

	while(eventsPending-- > 0){ // Deplete number of events
		XNextEvent(mContext.display, &currentEvent);

		// TODO: Perform event processing logic here
	}
}

bool Platform::getCursorCoords(float* xPos, float* yPos) {
	return true;
}

#endif
