#include "Platform.hpp"

Input_KeyLogger Platform::keyLogger = Input_KeyLogger(); // explicit definition of KeyLogger class
Input_MouseLogger Platform::mouseLogger = Input_MouseLogger(); // explicit definition of MouseLogger class
float Platform::xCursorPos = BAD_CURSOR_POS;
float Platform::yCursorPos = BAD_CURSOR_POS;

#ifdef _WIN32

LRESULT CALLBACK eventProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
	case (WM_LBUTTONDOWN): {
		if(wParam & MK_LBUTTON)
		(Platform::getCursorX() == BAD_CURSOR_POS || Platform::getCursorY() == BAD_CURSOR_POS)
		? Platform::mouseLogger.addMousePress(MOUSE_LeftBtn_Down)
		: Platform::mouseLogger.addMousePress(MOUSE_LeftBtn_Down, Platform::getCursorX(), Platform::getCursorY());
	}
	case (WM_LBUTTONUP): {
		// if(wParam & MK_LBUTTON)
		(Platform::getCursorX() == BAD_CURSOR_POS || Platform::getCursorY() == BAD_CURSOR_POS)
		? Platform::mouseLogger.addMousePress(MOUSE_LeftBtn_Up)
		: Platform::mouseLogger.addMousePress(MOUSE_LeftBtn_Up, Platform::getCursorX(), Platform::getCursorY()); 
	}
	case (WM_RBUTTONDOWN): {
		if(wParam & MK_RBUTTON)
		(Platform::getCursorX() == BAD_CURSOR_POS || Platform::getCursorY() == BAD_CURSOR_POS)
		? Platform::mouseLogger.addMousePress(MOUSE_RightBtn_Down)
		: Platform::mouseLogger.addMousePress(MOUSE_RightBtn_Down, Platform::getCursorX(), Platform::getCursorY());
	}
	case (WM_RBUTTONUP): { 
		// if(wParam & MK_RBUTTON)
		(Platform::getCursorX() == BAD_CURSOR_POS || Platform::getCursorY() == BAD_CURSOR_POS)
		? Platform::mouseLogger.addMousePress(MOUSE_RightBtn_Up)
		: Platform::mouseLogger.addMousePress(MOUSE_RightBtn_Up, Platform::getCursorX(), Platform::getCursorY());
	 }
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

void Platform::createWindow(){
    _context.windowClass = { 0 };
	_context.windowClass.hInstance = GetModuleHandle(NULL);
	_context.windowClass.lpfnWndProc = eventProc;
	_context.windowClass.lpszClassName = "Topl";
	RegisterClass(&_context.windowClass);

	_context.window = CreateWindow(
		"Topl",
		_winName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(_context.window, 1);
	UpdateWindow(_context.window);
}

void Platform::handleEvents(){
	if(!getCursorCoords(&Platform::xCursorPos, &Platform::yCursorPos)){
		xCursorPos = BAD_CURSOR_POS;
		yCursorPos = BAD_CURSOR_POS;
	} // sets cursor positions back to default values if out of window bounds

    while (PeekMessage(&_context.eventMsg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&_context.eventMsg);
		DispatchMessage(&_context.eventMsg);
	}
	if (_context.eventMsg.message == WM_QUIT) return; // Error code?
}

bool Platform::getCursorCoords(float* xPos, float* yPos) const { // Optimize this function
	POINT point;
	GetCursorPos(&point);

	RECT windowRect;
	GetWindowRect(_context.window, &windowRect);
	
	// Check if cursor is inside screen space
	if (point.x < windowRect.right && point.x > windowRect.left && point.y > windowRect.top && point.y < windowRect.bottom) {
		long unsigned halfWidth = ((windowRect.right - windowRect.left) / 2);
		LONG centerX = windowRect.left + halfWidth;
		*xPos = (point.x - centerX) / (float)halfWidth;

		long unsigned halfHeight = ((windowRect.bottom - windowRect.top) / 2);
		LONG centerY = windowRect.top + halfHeight;
		*yPos = -1.0f * (point.y - centerY) / (float)halfHeight;

		return true;
	} else return false; // Cursor is outside the screen space!
}

#elif defined(__linux__)

void Platform::createWindow(const char* windowName){
    _context.display = XOpenDisplay(NULL);

	GLint visualInfoAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo* visualInfo = glXChooseVisual(_context.display, 0, visualInfoAttribs);

	XSetWindowAttributes windowAttribs;
	windowAttribs.colormap = XCreateColormap(_context.display, DefaultRootWindow(_context.display), visualInfo->visual, AllocNone);
	windowAttribs.event_mask = ExposureMask | KeyPressMask;

    _context.window = XCreateWindow(
		_context.display, DefaultRootWindow(_context.display),
        0, 0,
        TOPL_WIN_WIDTH , TOPL_WIN_HEIGHT,
        0, visualInfo->depth,ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask | KeyPressMask
        InputOutput, visualInfo->visual,
        CWColormap | CWEventMask,
        &windowAttribs
	);

	XSelectInput(_context.display, _context.window, );
	XMapWindow(_context.display, _context.window);
}

void Platform::handleEvents(){
    int eventsPending = XEventsQueued(_context.display, QueuedAfterReading);
	XEvent currentEvent;

	while(eventsPending-- > 0){ // Deplete number of events
		XNextEvent(_context.display, &currentEvent);

		switch(currentEvent.type){
		// case (KeyPress): { Platform::keyLogger.addKeyPress((char)currentEvent.xkey.keycode); } // keycode needs to be converted!
		case (ButtonPress): {  }
		case (MotionNotify): {  }
		}
	}
}

bool Platform::getCursorCoords(float* xPos, float* yPos) {
	// TODO: Implement cursor tracking here!
	return true;
}

#endif
