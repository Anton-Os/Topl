#include "Platform.hpp"

Input_KeyControl Platform::keyControl = Input_KeyControl(); // explicit definition of KeyControl class
Input_MouseControl Platform::mouseControl = Input_MouseControl(); // explicit definition of MouseControl class

float Platform::xCursorPos = INVALID_CURSOR_POS;
float Platform::yCursorPos = INVALID_CURSOR_POS;

#ifdef _WIN32

static void addMousePress(enum MOUSE_Button button){
	(Platform::getCursorX() == INVALID_CURSOR_POS || Platform::getCursorY() == INVALID_CURSOR_POS)
		? Platform::mouseControl.addMousePress(button)
		: Platform::mouseControl.addMousePress(button, Platform::getCursorX(), Platform::getCursorY());
}

LRESULT CALLBACK eventProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(window);
	RECT rect;

	switch (message) {
	case(WM_CREATE): {
		// Create Function Here
	}
	case(WM_PAINT): {}
	case(WM_KEYDOWN): {}
	case(WM_KEYUP): {}
	case(WM_MOUSEMOVE):{}
	case (WM_CHAR): { if(wParam != 0) Platform::keyControl.addKeyPress((char)wParam); }
	case (WM_LBUTTONDOWN): { if(message == WM_LBUTTONDOWN) addMousePress(MOUSE_LeftBtn_Down); }
	case (WM_LBUTTONUP): { if(message == WM_LBUTTONUP) addMousePress(MOUSE_LeftBtn_Up); }
	case (WM_RBUTTONDOWN): { if(message == WM_RBUTTONDOWN) addMousePress(MOUSE_RightBtn_Down); }
	case (WM_RBUTTONUP): { if(message == WM_RBUTTONUP) addMousePress(MOUSE_RightBtn_Up); }
	default:
		return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}

void Platform::createWindow(unsigned width, unsigned height){
	LPTSTR iconResStr = MAKEINTRESOURCE(IDI_ICON);
	// LPTSTR menuResStr = MAKEINTRESOURCE(IDC_MENU);

    _context.windowClass = { 0 };
	_context.windowClass.hInstance = GetModuleHandle(NULL);
	_context.windowClass.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
	_context.windowClass.lpfnWndProc = eventProc;
	_context.windowClass.lpszClassName = "Topl";
	// _context.windowClass.lpszMenuName = MAKEINTRESOURCE(IDC_MENU);
	RegisterClass(&_context.windowClass);

	_context.window = CreateWindow(
		"Topl",
		_winName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height, // TOPL_WIN_WIDTH, TOPL_WIN_HEIGHT,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(_context.window, 1);
	UpdateWindow(_context.window);
}

void Platform::handleEvents(bool isCursorUpdate){
	if(isCursorUpdate == ENABLE_CURSOR_UPDATE) {	
		bool isCursorBound = getCursorCoords(&Platform::xCursorPos, &Platform::yCursorPos);
		if(!isCursorBound) resetCursor();
		else Platform::mouseControl.addHover(Platform::xCursorPos, Platform::yCursorPos); // handle hover callbacks
	}

    while (PeekMessage(&_context.eventMsg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&_context.eventMsg);
		DispatchMessage(&_context.eventMsg);
	}
	if (_context.eventMsg.message == WM_QUIT) return; // Error code?
}

unsigned Platform::getViewportHeight(NATIVE_WINDOW window){
	RECT rect;
	GetClientRect(window, &rect);
	return rect.right - rect.left;
}

unsigned Platform::getViewportWidth(NATIVE_WINDOW window){
	RECT rect;
	GetClientRect(window, &rect);
	return rect.bottom - rect.top;
}

bool Platform::getCursorCoords(float* xPos, float* yPos) const { // Optimize this function
	POINT point;
	GetCursorPos(&point);

	RECT windowRect;
	GetWindowRect(_context.window, &windowRect);
	
	if (point.x < windowRect.right && point.x > windowRect.left && point.y > windowRect.top && point.y < windowRect.bottom) {
		long unsigned halfWidth = ((windowRect.right - windowRect.left) / 2);
		LONG centerX = windowRect.left + halfWidth;
		*xPos = (point.x - centerX) / (float)halfWidth;

		long unsigned halfHeight = ((windowRect.bottom - windowRect.top) / 2);
		LONG centerY = windowRect.top + halfHeight;
		*yPos = -(point.y - centerY) / (float)halfHeight;
		if (*yPos > 0.0 && *yPos < 1.0) *yPos *= 1.05;  // positive y position adjusted

		return true;
	} else return false; // cursor outside the screen space!
}

#elif defined(__linux__)

void Platform::createWindow(unsigned width, unsigned height){
    _context.display = XOpenDisplay(NULL);

	/* GLint visualInfoAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo* visualInfo = glXChooseVisual(_context.display, 0, visualInfoAttribs); */
	int screenNum = DefaultScreen(_context.display);
	int depth = DefaultDepth(_context.display, screenNum);
	Visual* visual = DefaultVisual(_context.display, screenNum);

	XSetWindowAttributes windowAttribs;
	windowAttribs.colormap = XCreateColormap(_context.display, DefaultRootWindow(_context.display), visual, AllocNone);
	windowAttribs.border_pixel = 0;
	windowAttribs.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                        PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
                        ExposureMask | FocusChangeMask | VisibilityChangeMask |
                        EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

    _context.window = XCreateWindow(
		_context.display, DefaultRootWindow(_context.display),
        0, 0,
        width, height, // TOPL_WIN_WIDTH , TOPL_WIN_HEIGHT,
        0, depth,
        InputOutput,
		visual,
        CWBorderPixel | CWColormap | CWEventMask,
        &windowAttribs
	);

	Atom nameAtom = XInternAtom(_context.display, "_NET_WM_NAME", False);
	Atom iconAtom = XInternAtom(_context.display, "_NET_WM_ICON_NAME", False);

	XChangeProperty( // changing window title
		_context.display, _context.window,
		nameAtom, XInternAtom(_context.display, "UTF8_STRING", False),
		8, PropModeReplace,
		(const unsigned char*)_winName, strlen(_winName)
	);

	// XChangeProperty( /* changing window icon */ );

	// XSelectInput(_context.display, _context.window, ExposureMask);
	XMapWindow(_context.display, _context.window);
	XFlush(_context.display);
}

void Platform::handleEvents(bool isCursorUpdate){
	if (isCursorUpdate == ENABLE_CURSOR_UPDATE) {
		bool isCursorBound = getCursorCoords(&Platform::xCursorPos, &Platform::yCursorPos);
		if(!isCursorBound) resetCursor();
		else Platform::mouseControl.addHover(Platform::xCursorPos, Platform::yCursorPos); // handle hover callbacks
	}

    int eventsPending = XEventsQueued(_context.display, QueuedAfterReading);
	XEvent event;

	while(eventsPending-- > 0){ // Deplete number of events
		XNextEvent(_context.display, &event);

		switch(event.type){
		case (KeyPress): {
			// const char key = translateKey(event.xkey.keycode); // see GLFW x11_window.c line 1258
			Platform::keyControl.addKeyPress((char)event.xkey.keycode); // keycode needs to be converted!
			printf("Key press: %c \n", (char)event.xkey.keycode);
		}
		case (ButtonPress): { printf("Button press at %.5f, %.5f \n", Platform::xCursorPos, Platform::yCursorPos); }
		case (MotionNotify): { }
		}
	}
}

unsigned Platform::getViewportHeight(NATIVE_WINDOW window){
	return TOPL_WIN_HEIGHT; // TODO: get height here
}

unsigned Platform::getViewportWidth(NATIVE_WINDOW window){
	return TOPL_WIN_WIDTH; // TODO: get height here
}

bool Platform::getCursorCoords(float* xPos, float* yPos) const {
	Window root;
	Window window = DefaultRootWindow(_context.display);
	int xRoot, yRoot, xChild, yChild;
    unsigned int mask;

	XQueryPointer(
		_context.display, window, &root, &window,
		&xRoot, &yRoot, &xChild, &yChild,
		&mask
	);

	*xPos = xChild; // x needs to be translated!
	*yPos = yChild; // y needs to be translated!

	return true; // check if cursor is in client area!
}

#else // No Support

void Platform::createWindow(){ }

void Platform::handleEvents(bool isCursorUpdate){ }

bool Platform::getCursorCoords(float* xPos, float* yPos) const {}

#endif