#include "Platform.hpp"

namespace Topl {
    bool isInWindow = true;
    float cursorX; 
    float cursorY;
}

void mouseBtnCallback(){ 
	if (Topl::isInWindow) printf("Mouse press!\n\tCursor X: %f\n\tCursor Y: %f", Topl::cursorX, Topl::cursorY);
	else puts("Outside Window!");
}

int main(int argc, char** argv){
    Platform platform(argv[0]);
    platform.createWindow("Cursor");
    // platform.setupMainWindow(mainWindow);

    platform.mouseLogger.addCallback(MOUSE_LeftBtn_Down, mouseBtnCallback);
    platform.mouseLogger.addCallback(MOUSE_RightBtn_Down, mouseBtnCallback);

    while(true){
		Topl::isInWindow = platform.getCursorCoords(&Topl::cursorX, &Topl::cursorY);
        platform.handleEvents();
    }

    return 0;
}