#include "native_os_def.h"

#include "FileIO.hpp";
#include "Timer.hpp";
#include "Input.hpp";

struct Platform {
    Platform(const char* execPath){
        mExecPath = execPath;
    }

    NATIVE_WINDOW createWindow(const char* windowName);
    void setupMainWindow(NATIVE_WINDOW window);
    void handleEvents();

    Timer_Ticker* mTicker;
    Input_KeyLogger* mKeyLogger;
    Input_MouseLogger* mMouseLogger;
private:
    const char* mExecPath; // full path to executable
    NATIVE_PLATFORM_CONTEXT mContext;
};