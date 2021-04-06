#include "Platform.hpp"

namespace Topl {
    bool isInWindow;
    float cursorX; 
    float cursorY;
}

void mouseBtnCallback(){ 
    if(Topl::isInWindow) printf("Mouse press!\n\tCursor X: %f\n\tCursor Y: %f", Topl::cursorX, Topl::cursorY);
}

int main(int argc, char** argv){
    Platform platform(argv[0]);
    NATIVE_WINDOW mainWindow = platform.createWindow("Cursor");
    platform.setupMainWindow(mainWindow);

    platform.mouseLogger.addCallback(MOUSE_LeftBtn_Down, mouseBtnCallback);
    platform.mouseLogger.addCallback(MOUSE_RightBtn_Down, mouseBtnCallback);

    while(true){
        platform.handleEvents();
    }

    return 0;
}