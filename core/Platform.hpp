#include "native_os_def.h"

#include "FileIO.hpp";
#include "Timer.hpp";
#include "Input.hpp";

struct Platform {
    Platform(const char* execPath){
        mExecPath = execPath;
    }

    void createWindow(const char* windowName);
    // void setupMainWindow(NATIVE_WINDOW window); // For displaying the target Window
    void handleEvents(); // handles platform specific events
	bool getCursorCoords(float* xPos, float* yPos); // returns false if not inside window

    // static Timer_Ticker mTicker;
    NATIVE_WINDOW getNativeWindow(){ return mContext.window; }
    static Input_KeyLogger keyLogger;
    static Input_MouseLogger mouseLogger;
private:
    const char* mExecPath; // full path to executable
    NATIVE_PLATFORM_CONTEXT mContext;
};