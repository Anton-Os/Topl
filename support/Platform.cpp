#include "Platform.hpp"

Input_KeyControl Platform::keyControl = Input_KeyControl(); // explicit definition of KeyControl class
Input_MouseControl Platform::mouseControl = Input_MouseControl(); // explicit definition of MouseControl class

float Platform::xCursorPos = INVALID_CURSOR_POS;
float Platform::yCursorPos = INVALID_CURSOR_POS;

char Platform::inputStr[1024] = "";
bool Platform::isUserInput = false;
resizeCallback Platform::onResize = nullptr;
fileCallback Platform::onFileChoose = nullptr;

#ifdef _WIN32

// bool checkFile(std::string fileName){ return (access(fileName.c_str(), F_OK) == 0)? true : false; }

static bool checkKey(int code){
	return isalnum(code) || isspace(code) || code == '\r' || // handles most usecases
		   code == (char)0x25 || code == (char)0x26 || code == (char)0x27 || code == (char)0x28; // handles arrows
}

static void addPress(enum MOUSE_Event button){
	(Platform::getCursorX() == INVALID_CURSOR_POS || Platform::getCursorY() == INVALID_CURSOR_POS)
		? Platform::mouseControl.addPress(button)
		: Platform::mouseControl.addPress(button, Platform::getCursorX(), Platform::getCursorY());
}

struct DropTarget_Win32 : public IDropTarget {
	DropTarget_Win32() : IDropTarget(){}

	ULONG AddRef()  { return 1; }
	ULONG Release() { return 0; }

	HRESULT QueryInterface(REFIID riid, void **ppvObject){
		if (riid == IID_IDropTarget) std::cout << "IDropTarget id detected";
		return S_OK; 
	}

	HRESULT DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override { std::cout << "DragEnter callback event" << std::endl; return S_OK; }
	HRESULT DragLeave(void) override { std::cout << "DragLeave callback event" << std::endl; return S_OK; }
	HRESULT DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override { std::cout << "DragOver callback event" << std::endl; return S_OK; }
	HRESULT Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override { std::cout << "Drop callback event" << std::endl; return S_OK; }
} dropTarget;

LRESULT handleMenu_win32(WPARAM wParam){
	switch(LOWORD(wParam)){
		case IDM_NEW: 
			Platform::openFileDialog(false); // Testing
			if(Platform::onFileChoose != nullptr) Platform::onFileChoose(false, "../"); // TODO: Include real path
			logMessage("Menu command: New");
			break;
		case IDM_LOAD: 
			Platform::openFileDialog(true); // Testing
			if(Platform::onFileChoose != nullptr) Platform::onFileChoose(true, "../"); // TODO: Include real path
			logMessage("Menu command: Load\n"); 
			break;
		case IDM_TIME: logMessage("Menu command: Timeline\n"); break;
		case IDM_OBJS: logMessage("Menu command: Objects\n"); break;
		case IDM_PROPS: logMessage("Menu command: Properties\n"); break;
		default: break;
		// case IDM_FI_CLOSE: logMessage("File close command"); break;
	}

	return 0;
}

void Platform::openFileDialog(bool isRead){
	IFileOpenDialog *fileDialog_win32;	

	if(SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&fileDialog_win32))))
		if(SUCCEEDED(fileDialog_win32->Show(NULL))){
			logMessage("Showing FileDialog object success! ");

			IShellItem* shellItem_win32;
			if(SUCCEEDED(fileDialog_win32->GetResult(&shellItem_win32))){
				PWSTR filePath_win32;
				if(SUCCEEDED(shellItem_win32->GetDisplayName(SIGDN_FILESYSPATH, &filePath_win32))){
					OutputDebugStringW(&(*filePath_win32));
					CoTaskMemFree(filePath_win32);
				}
				shellItem_win32->Release();
			}
			fileDialog_win32->Release();
		}

}

LRESULT CALLBACK eventProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	static bool isKeyReady = false;

	switch (message) {
	case(WM_COMMAND): { return handleMenu_win32(wParam); }
	case(WM_CREATE): {}
	case(WM_PAINT): { }
	// case(WM_SIZE): { printf("Viewport resized to: %d, %d", Platform::getViewportWidth(window), Platform::getViewportHeight(window)); }
	case(WM_KEYDOWN): { isKeyReady = true; }
	case(WM_KEYUP): {}
	case(WM_MOUSEMOVE):{
		if(Platform::getCursorX() != INVALID_CURSOR_POS && Platform::getCursorY() != INVALID_CURSOR_POS)
			Platform::mouseControl.addHover(Platform::getCursorX(), Platform::getCursorY());
	}
	case (WM_CHAR): { 
		if(wParam == VK_ESCAPE) std::cout << "Escape pressed" << std::endl;
		else if(isKeyReady && checkKey(wParam)) Platform::keyControl.addKeyPress(wParam);
		isKeyReady = false;
	}
	case (WM_LBUTTONDOWN): { if(message == WM_LBUTTONDOWN) addPress(MOUSE_LeftBtn_Press); }
	case (WM_LBUTTONUP): { if(message == WM_LBUTTONUP) addPress(MOUSE_LeftBtn_Release); }
	case (WM_RBUTTONDOWN): { if(message == WM_RBUTTONDOWN) addPress(MOUSE_RightBtn_Press); }
	case (WM_RBUTTONUP): { if(message == WM_RBUTTONUP) addPress(MOUSE_RightBtn_Release); }
	case (WM_MOUSEWHEEL): { 
		if(message == WM_MOUSEWHEEL && Platform::mouseControl.onScroll != nullptr) 
			Platform::mouseControl.onScroll(GET_WHEEL_DELTA_WPARAM(wParam) > 0);
	}
	case (WM_DROPFILES): {
		if(message == WM_DROPFILES) std::cout << "Files dropped triggered" << std::endl;
		// if(Platform::onFileChoose != nullptr) Platform::onFileChoose()
	} 
	/* case (WM_MBUTTONDOWN): { if(message == WM_MBUTTONDOWN) std::cout << "Scroll Down" << std::endl; }
	case (WM_MBUTTONUP): { if(message == WM_MBUTTONUP) std::cout << "Scroll Up" << std::endl; } */
	default:
		return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}

void Platform::createWindow(unsigned width, unsigned height){
	LPTSTR iconResStr = MAKEINTRESOURCE(IDI_ICON);
	LPTSTR menuResStr = MAKEINTRESOURCE(IDC_MENU);

    _context.windowClass = { 0 };
	_context.windowClass.hInstance = GetModuleHandle(NULL);
	_context.windowClass.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
	_context.windowClass.lpfnWndProc = eventProc;
	_context.windowClass.lpszClassName = "Topl";
	_context.windowClass.lpszMenuName = MAKEINTRESOURCE(IDC_MENU);
	RegisterClass(&_context.windowClass);

	LPCTSTR cursor = IDC_ARROW;
    auto hCursor = LoadCursor(NULL, cursor);
    SetCursor(hCursor);

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

	DragAcceptFiles(_context.window, TRUE);
	RegisterDragDrop(_context.window, &dropTarget);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
}

bool Platform::handleEvents(){
	bool isCursorBound = getCursorCoords(&Platform::xCursorPos, &Platform::yCursorPos);
	
	if(isCursorBound) 
		(Platform::mouseControl.getIsMouseDown().second)
			? Platform::mouseControl.addDrag(Platform::xCursorPos, Platform::yCursorPos)
			: Platform::mouseControl.addHover(Platform::xCursorPos, Platform::yCursorPos);
	else resetCursor();

    while (PeekMessage(&_context.eventMsg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&_context.eventMsg);
		DispatchMessage(&_context.eventMsg);

		if (_context.eventMsg.message == WM_QUIT || _context.eventMsg.message == WM_CLOSE) return false; // Error code?
	}

	/* if(Platform::isUserInput){
		puts("Enter a command: ");
		scanf("%s", &Platform::inputStr);
		Platform::isUserInput = false;
	} */

	return true;
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

	RECT rect;
	GetWindowRect(_context.window, &rect);
	rect.top += 45; // this adjustment fixes issues
	
	if (point.x < rect.right && point.x > rect.left && point.y > rect.top && point.y < rect.bottom) {
		long unsigned halfWidth = ((rect.right - rect.left) / 2);
		LONG centerX = rect.left + halfWidth;
		if(xPos != nullptr) *xPos = (point.x - centerX) / (float)halfWidth;

		long unsigned halfHeight = ((rect.bottom - rect.top) / 2);
		LONG centerY = rect.top + halfHeight;
		if(yPos != nullptr) *yPos = -(point.y - centerY) / (float)halfHeight;

		return true;
	} else return false; // cursor outside the screen space!
}

#elif defined(__ANDROID__)

NATIVE_WINDOW window = nullptr;

void android_proc(android_app *app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: window = app->window; break;
        case APP_CMD_TERM_WINDOW: break;
        default: break;
    }
}

void Platform::createWindow(unsigned width, unsigned height){
	if(window != nullptr) _context.window = window;
}

bool Platform::handleEvents(){
    static int events;
    static android_poll_source* pollSrc;

	if (ALooper_pollAll(0, nullptr, &events, (void **) &pollSrc) >= 0) {
		if (pollSrc) {
			pollSrc->process(pApp, pollSrc);
		}
	}

    return true;
}

bool Platform::getCursorCoords(float* xPos, float* yPos) const { return false; }

unsigned Platform::getViewportHeight(NATIVE_WINDOW window){
    return TOPL_WIN_HEIGHT; // TODO: get height here
}

unsigned Platform::getViewportWidth(NATIVE_WINDOW window){
    return TOPL_WIN_WIDTH; // TODO: get width here
}

#elif defined(__linux__)

void Platform::createWindow(unsigned width, unsigned height){
    XInitThreads();
    _context.display = XOpenDisplay(NULL);

    /* int screenNum = DefaultScreen(_context.display);
    int depth = DefaultDepth(_context.display, screenNum);
    Visual* visual = DefaultVisual(_context.display, screenNum); */

    GLint visualInfoAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XVisualInfo* visualInfo = glXChooseVisual(_context.display, 0, visualInfoAttribs);
    if(visualInfo == nullptr) return logMessage(MESSAGE_Exclaim, "Issue with GLX Visual");
    else _context.visualInfo = visualInfo;

    XSetWindowAttributes windowAttribs = {};
    windowAttribs.colormap = XCreateColormap(_context.display, DefaultRootWindow(_context.display), visualInfo->visual, AllocNone);
    // windowAttribs.colormap = XCreateColormap(_context.display, DefaultRootWindow(_context.display), visual, AllocNone);
    windowAttribs.border_pixel = 0;
    windowAttribs.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                        PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
                        ExposureMask | FocusChangeMask | VisibilityChangeMask |
                        EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

    _context.window = XCreateWindow(
        _context.display, DefaultRootWindow(_context.display),
        0, 0,
        width, height, // TOPL_WIN_WIDTH , TOPL_WIN_HEIGHT,
        0, visualInfo->depth,
        InputOutput,
        visualInfo->visual,
        CWColormap | CWEventMask,
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

bool Platform::handleEvents(){
    bool isCursorBound = getCursorCoords(&Platform::xCursorPos, &Platform::yCursorPos);
    (isCursorBound)? Platform::mouseControl.addHover(Platform::xCursorPos, Platform::yCursorPos) : resetCursor();

    int eventsPending = XEventsQueued(_context.display, QueuedAfterReading);
    XEvent event;

    while(eventsPending-- > 0){ // Deplete number of events
        XNextEvent(_context.display, &event);

        switch(event.type){
        case (KeyPress): {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            Platform::keyControl.addKeyPress((char)keysym); // keycode needs to be converted!
            printf("Key press: %c \n", (char)keysym);
        }
        case (ButtonPress): {
            printf("Button press at %.5f, %.5f \n", Platform::xCursorPos, Platform::yCursorPos);
        }
        case (ButtonRelease): {
            printf("Button release at %.5f, %.5f \n", Platform::xCursorPos, Platform::yCursorPos);
        }
        case (MotionNotify): { }
        /* case (DestroyNotify): {
           std::cout << "DestroyNotify event triggered" << std::endl;
           return false;
        } */
        }
    }

    return true;
}

unsigned Platform::getViewportHeight(NATIVE_WINDOW window){
    XWindowAttributes windowAttribs;
    // XGetWindowAttributes(XOpenDisplay(NULL), window, &windowAttribs);
    return TOPL_WIN_HEIGHT; // windowAttribs.height;
}

unsigned Platform::getViewportWidth(NATIVE_WINDOW window){
    XWindowAttributes windowAttribs;
    // XGetWindowAttributes(XOpenDisplay(NULL), window, &windowAttribs);
    return TOPL_WIN_WIDTH; // windowAttribs.width;
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

    unsigned height = Platform::getViewportHeight(_context.window);
    unsigned width = Platform::getViewportWidth(_context.window);

    *xPos = xChild; // x needs to be translated!
    *yPos = yChild; // y needs to be translated!

    return true; // check if cursor is in client area!
}

#endif
