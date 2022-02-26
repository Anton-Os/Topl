#ifndef TOPL_PLATFORM_H

#include "native_os_def.h"

#include "FileIO.hpp";
#include "Timer.hpp";
#include "Input.hpp";

struct Platform {
    Platform(const char* execPath, const char* winName){
        _execPath = execPath;
		_winName = winName;
        resetCursor();
    }

    void createWindow();
    void handleEvents(bool isCursorUpdate); // handles platform specific events, updates cursor position if arg true
    /* static void updateTimer(){
		if (_tstampMilli == 0.0f) _ticker.reset(); // first update requires timer reset
		else _tstampMilli = _ticker.getRelMillisecs(); // updates timer and timestamp
	} */

    NATIVE_WINDOW getParentWindow(){ return _context.window; }
    static float getCursorX(){ return xCursorPos; }
    static float getCursorY(){ return yCursorPos; }

    static Input_KeyLogger keyLogger;
    static Input_MouseLogger mouseLogger;
private:
    bool getCursorCoords(float* xPos, float* yPos) const; // returns true if within window bounds
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

    static double _tstamp; // timestamp for most recent event
    Timer_Ticker _ticker; // used for internal updates
};

#define TOPL_PLATFORM_H
#endif