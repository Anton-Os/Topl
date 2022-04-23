#include "Topl_Scene.hpp"

int main(int argc, char** argv){
    std::string sceneFile = std::string(ASSETS_DIR) + "res/" + "MyScene.tp";
    Topl_Scene savedScene = Topl_Scene(sceneFile); // testing load from file
}