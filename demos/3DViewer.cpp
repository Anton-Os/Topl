#include "3DViewer.hpp"

void Viewer_Demo::init(){
    // TODO: Include init code
}

void Viewer_Demo::loop(double frameTime){
    // TODO: Include looping code
}

int main(int argc, char** argv) {
    _instance = new Viewer_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}