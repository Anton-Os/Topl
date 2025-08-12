#include "meshes/Geo_Volume.hpp"
#include "constructs/Geo_Puppet.hpp"
#ifdef TOPL_ENABLE_MODELS
#include "constructs/Geo_Model3D.hpp"
#endif

#include "program/Topl_Program.hpp"

#define ANIMATRIX_PUPPETS 3
#define ANIMATRIX_MODELS 4
#define ANIMATRIX_FLOOR_SIZE 20.0F

struct Animatrix_Demo : public Topl_Program {
    Animatrix_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Animatrix", backend){}

    void init() override;
    void loop(double frameTime) override;
private:
    void onAnyKey(char key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;
    std::string getPuppetImgPath(std::string appendageStr){ return std::string(IMAGES_DIR) + "characters/" + appendageStr; }

    std::string puppetAssetPaths[ANIMATRIX_PUPPETS][PUPPET_PARTS] = {
        { getPuppetImgPath("Angel-Torso.png"), getPuppetImgPath("Angel-Head.png"),
          getPuppetImgPath("Angel-LeftWing.png"), getPuppetImgPath("Angel-RightWing.png"),
          getPuppetImgPath("Angel-LeftLeg.png"), getPuppetImgPath("Angel-RightLeg.png") },
        { getPuppetImgPath("Ghost-Torso.png"), getPuppetImgPath("Ghost-Head.png"),
          getPuppetImgPath("Ghost-LeftArm.png"), getPuppetImgPath("Ghost-RightArm.png"),
          getPuppetImgPath("Ghost-LeftLeg.png"), getPuppetImgPath("Ghost-RightLeg.png") },
        { getPuppetImgPath("Demon-Torso.png"), getPuppetImgPath("Demon-Head.png"),
          getPuppetImgPath("Demon-LeftWing.png"), getPuppetImgPath("Demon-RightWing.png"),
          getPuppetImgPath("Demon-LeftLeg.png"), getPuppetImgPath("Demon-RightLeg.png") }
    };

    Geo_Puppet2D puppets[ANIMATRIX_PUPPETS] = { 
      Geo_Puppet2D("puppet1", puppetAssetPaths[0]), // angel
      Geo_Puppet2D("puppet2", puppetAssetPaths[1]), // ghost
      Geo_Puppet2D("puppet3", puppetAssetPaths[2])  // demon
    };
    std::vector<Phys_Connector> anchors;

    Geo_Mesh floorMesh = Geo_Quad3D(1.0, ANIMATRIX_FLOOR_SIZE);
    Geo_Actor floor = Geo_Actor("floor", &floorMesh);
#ifdef TOPL_ENABLE_MODELS
    std::string modelPath = std::string(std::string(MODELS_DIR) + "SpinTop.obj");
    Geo_Model3D models[ANIMATRIX_MODELS] = { Geo_Model3D("model1", modelPath), Geo_Model3D("model2", modelPath), Geo_Model3D("model3", modelPath), Geo_Model3D("model4", modelPath) };
#endif

    Topl_Scene scene2D = PROGRAM_SCENE; 
    Topl_Scene scene3D = PROGRAM_SCENE;
} *Animatrix;