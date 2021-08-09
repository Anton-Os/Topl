#ifndef TOPL_PLATFORM_H

#include "native_os_def.h"

#include "GUI.h"
#include "FileIO.hpp";
#include "Timer.hpp";
#include "Input.hpp";

struct Platform {
    Platform(const char* execPath){
        _execPath = execPath;
    }

    void createWindow(const char* windowName);
    void createQuadGUI(NATIVE_WINDOW parentWindow, UI_QuadLayout quadLayout);
	void createTreeGUI(NATIVE_WINDOW parentWindow /* UI_QuadLayout quadLayout */); // use UI_TreeLayout argument
    void handleEvents(); // handles platform specific events

    // static Timer_Ticker mTicker;
    HWND getNativeWindow(){ return _context.window; }
    bool getCursorCoords(float* xPos, float* yPos) const; // returns true if within window bounds
    
    static Input_KeyLogger keyLogger;
    static Input_MouseLogger mouseLogger;
private:
    const char* _execPath; // full path to executable
    NATIVE_PLATFORM_CONTEXT _context;
};

#define TOPL_PLATFORM_H
#endif