#ifndef TOPL_PLATFORM_H

#include <bitset>
#include <limits>
#include <thread>

#include "native_os_def.h"

#include "IO.hpp"
#include "Controls.hpp"

typedef void (*resizeCallback)(unsigned, unsigned);
typedef void (*fileCallback)(bool, const char*);

bool checkFile(std::string fileName);

struct Platform {
#ifndef __ANDROID__
    Platform(const char* execPath, const char* winName){
        _execPath = execPath;
		_winName = winName;
    }
#else
    Platform(android_app* app){
        _context.app = app;
        _context.app->onAppCmd = android_proc;
    }
#endif

    void createWindow(unsigned width, unsigned height);
    bool handleEvents(); // handles platform specific events until end is reached

    NATIVE_PLATFORM_CONTEXT* getContext(){ return &_context; }
    NATIVE_WINDOW getParentWindow(){ return _context.window; }
    static unsigned getViewportWidth(NATIVE_WINDOW window);
    static unsigned getViewportHeight(NATIVE_WINDOW window);
    static float getCursorX(){ return xCursorPos; }
    static float getCursorY(){ return yCursorPos; }
    static void openFileDialog(bool isRead); // opens file dialog

    static Input_KeyControl keyControl;
    static Input_MouseControl mouseControl;

    static char inputStr[1024];
    static bool isUserInput;

    static resizeCallback onResize;
    static fileCallback onFileChoose;
private:
    bool getCursorCoords(float* xPos, float* yPos) const; // returns true within client area and false outside
    void resetCursor(){
        xCursorPos = INVALID_CURSOR_POS;
		yCursorPos = INVALID_CURSOR_POS;
    }
    
    NATIVE_PLATFORM_CONTEXT _context;
	const char* _execPath; // full path to executable
	const char* _winName; // parent window display name
    // static bool isCursorInClient; // internally tracks if cursor is in client space
    static float xCursorPos; // internally tracks cursor position along x axis
    static float yCursorPos; // internally tracks cursor position along y axis
};

#define TOPL_PLATFORM_H
#endif
