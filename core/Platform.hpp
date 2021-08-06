#ifndef TOPL_PLATFORM_H

#include "native_os_def.h"

#include "FileIO.hpp";
#include "Timer.hpp";
#include "Input.hpp";

struct Platform {
    Platform(const char* execPath){
        mExecPath = execPath;
    }

    void createWindow(const char* windowName);
    void createChildGUI(NATIVE_WINDOW parentWindow); // provide GUI customization options!
    void handleEvents(); // handles platform specific events

    // static Timer_Ticker mTicker;
    HWND getNativeWindow(){ return mContext.window; }
    bool getCursorCoords(float* xPos, float* yPos); // returns false if not inside window
    
    static Input_KeyLogger keyLogger;
    static Input_MouseLogger mouseLogger;
private:
    const char* mExecPath; // full path to executable
    NATIVE_PLATFORM_CONTEXT mContext;
};

#define TOPL_PLATFORM_H
#endif