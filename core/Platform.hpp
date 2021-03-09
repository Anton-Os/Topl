#include "native_os_def.h"

#include "FileIO.hpp";
#include "Timer.hpp";
#include "Input.hpp";

struct Platform {
    Platform(){}

    NATIVE_WINDOW getMainWindow();

    void createMainWindow(const char* windowName);
    void handleEvents();

    Timer_Ticker ticker;
    Input_KeyLogger keyLogger;
    Input_MouseLogger mouseLogger;
    NATIVE_PLATFORM_CONTEXT context;
};