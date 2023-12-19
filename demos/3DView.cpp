#include "3DView.hpp"

void 3DView_Demo::init(){
    // TODO: Include init code
}

void 3DView_Demo::lop(double frameTime){
    // TODO: Include looping code
}

int main(int argc, char** argv) {
    _instance = new 3DView_Demo(argv[0], BACKEND_GL4);
    _instance->run();

    delete(_instance);
    return 0;
}