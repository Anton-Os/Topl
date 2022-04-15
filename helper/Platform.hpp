#ifndef TOPL_PLATFORM_H

#include "native_os_def.h"

#include "IO.hpp";
#include "Controls.hpp";

struct Platform {
    Platform(const char* execPath, const char* winName){
        _execPath = execPath;
		_winName = winName;
        resetCursor();
    }

    void createWindow();
    void handleEvents(bool isCursorUpdate); // handles platform specific events, updates cursor position if arg true
    // void handleCursorEvents(); // separate function may be required

    NATIVE_WINDOW getParentWindow(){ return _context.window; }
    static unsigned getViewportWidth(NATIVE_WINDOW window);
    static unsigned getViewportHeight(NATIVE_WINDOW window);
    static float getCursorX(){ return xCursorPos; }
    static float getCursorY(){ return yCursorPos; }

    static Input_KeyLogger keyLogger;
    static Input_MouseLogger mouseLogger;
private:
    bool getCursorCoords(float* xPos, float* yPos) const; // returns true within client area and false outside
    void resetCursor(){
        xCursorPos = BAD_CURSOR_POS;
		yCursorPos = BAD_CURSOR_POS;
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