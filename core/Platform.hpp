#ifndef TOPL_PLATFORM_H

#include "native_os_def.h"

#include "FileIO.hpp";
#include "Timer.hpp";
#include "Input.hpp";

struct Platform {
    Platform(const char* execPath, const char* winName){
        _execPath = execPath;
		_winName = winName;
    }

    void createWindow();
    void handleEvents(); // handles platform specific events

    HWND getParentWindow(){ return _context.window; }
	HWND getChildWindow(unsigned short childNum) {
		if (_windowCount <= 1 || childNum > _windowCount - 1) return 0; // problem retrieving child window
		else return _context.childWindows[childNum];
	}
    bool getCursorCoords(float* xPos, float* yPos) const; // returns true if within window bounds
    
    static Input_KeyLogger keyLogger;
    static Input_MouseLogger mouseLogger;
private:
	const char* _execPath; // full path to executable
	const char* _winName; // parent window display name
	NATIVE_PLATFORM_CONTEXT _context;

	unsigned short _windowCount = 0; // increments as new windows are created
	unsigned short _windowMax = TOPL_WIN_MAX_COUNT;
};

#define TOPL_PLATFORM_H
#endif