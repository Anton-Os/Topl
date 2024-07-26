#include "Molecular.hpp"

void Molecular_Demo::loop(){
    // TODO: Add initialization logic
}

void Molecular_Demo::loop(double frameTime){
    // TODO: Add updates and drawing
}

int main(int argc, char** argv) {
    _DEMO = new Molecular_Demo(argv[0], BACKEND_GL4);
    _DEMO->run();

    delete(_DEMO);
    return 0;
}