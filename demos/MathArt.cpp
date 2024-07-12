#include "MathArt.hpp"

void MathArt::init(){
    // TODO: Add init logic
}

void MathArt::loop(double frameTime){
    // TODO: Add loop logic
}

int main(int argc, char** argv) {
    _DEMO = new MathArt_Demo(argv[0], BACKEND_GL4);

    _DEMO->run();
    delete(_DEMO);
    return 0;
}