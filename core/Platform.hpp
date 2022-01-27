#ifndef TOPL_PLATFORM_H

#include "native_os_def.h"

#include "FileIO.hpp";
#include "Timer.hpp";
#include "Input.hpp";

#define BAD_CURSOR_POS -1.0f

struct Platform {
    Platform(const char* execPath, const char* winName){
        _execPath = execPath;
		_winName = winName;
    }

    void createWindow();
    void handleEvents(); // handles platform specific events

    NATIVE_WINDOW getParentWindow(){ return _context.window; }
    static float getCursorX(){ return xCursorPos; }
    static float getCursorY(){ return yCursorPos; }

    static Input_KeyLogger keyLogger;
    static Input_MouseLogger mouseLogger;
private:
    bool getCursorCoords(float* xPos, float* yPos) const; // returns true if within window bounds
    
    NATIVE_PLATFORM_CONTEXT _context;
	const char* _execPath; // full path to executable
	const char* _winName; // parent window display name
    static float xCursorPos; // internally tracks cursor position along x axis of window
    static float yCursorPos; // internally tracks cursor position along y axis of window
};

#define TOPL_PLATFORM_H
#endif