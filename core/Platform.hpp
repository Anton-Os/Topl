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

    // static Timer_Ticker mTicker;
    static Input_KeyLogger keyLogger;
    static Input_MouseLogger mouseLogger;
private:
    const char* mExecPath; // full path to executable
    NATIVE_PLATFORM_CONTEXT mContext;
};

// Input_KeyLogger Platform::keyLogger; // explicit definition of static member class
// Input_MouseLogger Platform::mouseLogger; // explicit definition of static member class