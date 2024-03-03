#ifndef TOPL_PLATFORM_H

#include <bitset>
#include <thread>

#include "native_os_def.h"

#include "IO.hpp";
#include "Controls.hpp";

#define DISABLE_CURSOR_UPDATE 0
#define ENABLE_CURSOR_UPDATE 1

struct Platform {
    Platform(const char* execPath, const char* winName){
        _execPath = execPath;
		_winName = winName;
        resetCursor();
    }

    void createWindow(unsigned width, unsigned height);
    bool handleEvents(); // handles platform specific events until end is reached

    NATIVE_WINDOW getParentWindow(){ return _context.window; }
    static unsigned getViewportWidth(NATIVE_WINDOW window);
    static unsigned getViewportHeight(NATIVE_WINDOW window);
    static float getCursorX(){ return xCursorPos; }
    static float getCursorY(){ return yCursorPos; }

    static Input_KeyControl keyControl;
    static Input_MouseControl mouseControl;

    static char inputStr[1024];
    static bool isUserInput;

    void (*resizeCallback)(unsigned, unsigned);
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