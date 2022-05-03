#include "Platform.hpp"

int main(int argc, char** argv){
    Platform platform = Platform(argv[0], "InputTracer");
    platform.createWindow();

    while (1) {
        platform.handleEvents(true);
    }

    return 0;
}