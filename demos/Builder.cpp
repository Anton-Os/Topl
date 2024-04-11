#include "Builder.hpp"

void Builder_Demo::init(){
    // Handle platform and callbacks
    // Handle objects and scene creation
    // Handle renderer initialization
}

void Builder_Demo::loop(double frameTime){
    // Handle object and shader updates
    // Handle object drawing
}

int main(int argc, char** argv) {
    _instance = new Builder_Demo(argv[0], BACKEND_DX11);
    _instance->run();

    delete(_instance);
    return 0;
}