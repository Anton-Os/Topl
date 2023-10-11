#include "Sandbox.hpp"

void Sandbox_Demo::init(){
    // TODO: Add initialization code
}

void Sandbox_Demo::loop(double frameTime){
    // TODO: Add game loop code
}

int main(int argc, char** argv) {
    Sandbox_Demo demo = Sandbox_Demo(argv[0], TARGET_OpenGL);

    demo.run();
    return 0;
}