
#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Volume.hpp"

#include "constructs/Geo_Puppet.hpp"
#include "constructs/Geo_Model3D.hpp"

#include "program/Topl_Program.hpp"

#define FLOOR_SIZE 100.0F
#define PILLAR_SIZE 1.0F

struct FirstPerson_Demo : public Topl_Program {
    FirstPerson_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "FirstPerson", backend){
        // 2D Objects

        puppet1.shift({ 0.0F, -1.0, -5.0F });
        puppet2.shift({ -1.0F, -1.0, -5.0F });
        puppet3.shift({ 1.0F, -1.0, -5.0F });

        anchorOffs[0] = { 0.0F, -1.0, 0.0F };
        anchorOffs[1] = { -1.0F, -1.0, 0.0F };
        anchorOffs[2] = { 1.0F, -1.0, 0.0F };

        // 3D Objects
        
        floor.setPos({ 0.0F, -3.0F, 0.0F});
        floor.setSize({ FLOOR_SIZE, 1.0F, FLOOR_SIZE });
        floor.setRot({ 0.0F, MATH_HALF_PI, 0.0F });

        pillars[0].setPos({ 10.0F, 2.5F, 10.0F });
        pillars[1].setPos({ -10.0F, 2.5F, 10.0F });
        pillars[2].setPos({ 10.0F, 2.5F, -10.0F });
        pillars[3].setPos({ -10.0F, 2.5F, -10.0F });
        for(unsigned short p = 0; p < 4; p++){ 
            pillars[p].setSize({ 1.0F, 10.0F, 1.0F });
            pillars[p].setRot({ 0.0F, MATH_HALF_PI, 0.0F });
        }

        roof.setRot({ 0.0F, -MATH_HALF_PI, 0.0F });
        roof.setPos({ 0.0F, 7.5F, 0.0F });
        roof.setSize({ 30.0, 3.0F, 30.0 });

        models[0].shift({ -1.5F, -1.75F, 1.0F });
        models[1].shift({ 0.0F, -1.75F, 0.0F });
        models[2].shift({ 1.5F, -1.75, 1.0F });
        models[3].shift({ -1.5F, -1.75F, -1.0F });
        models[4].shift({ 1.5F, -1.75, -1.0F });
    }

    void init() override;
    void loop(double frameTime) override;

    std::string ghostPuppetAssets[PUPPET_PARTS] = {
        std::string(IMAGES_DIR) + "characters/" + "Ghost-Torso.png", std::string(IMAGES_DIR) + "characters/" + "Ghost-Head.png",
		std::string(IMAGES_DIR) + "characters/" + "Ghost-LeftArm.png", std::string(IMAGES_DIR) + "characters/" + "Ghost-RightArm.png",
		std::string(IMAGES_DIR) + "characters/" + "Ghost-LeftLeg.png", std::string(IMAGES_DIR) + "characters/" + "Ghost-RightLeg.png"
    };
    std::string angelPuppetAssets[PUPPET_PARTS] = {
        std::string(IMAGES_DIR) + "characters/" + "Angel-Torso.png", std::string(IMAGES_DIR) + "characters/" + "Angel-Head.png",
		std::string(IMAGES_DIR) + "characters/" + "Angel-LeftWing.png", std::string(IMAGES_DIR) + "characters/" + "Angel-RightWing.png",
		std::string(IMAGES_DIR) + "characters/" + "Angel-LeftLeg.png", std::string(IMAGES_DIR) + "characters/" + "Angel-RightLeg.png"
    };
    std::string demonPuppetAssets[PUPPET_PARTS] = {
        std::string(IMAGES_DIR) + "characters/" + "Demon-Torso.png", std::string(IMAGES_DIR) + "characters/" + "Demon-Head.png",
		std::string(IMAGES_DIR) + "characters/" + "Demon-LeftWing.png", std::string(IMAGES_DIR) + "characters/" + "Demon-RightWing.png",
		std::string(IMAGES_DIR) + "characters/" + "Demon-LeftLeg.png", std::string(IMAGES_DIR) + "characters/" + "Demon-RightLeg.png"
    };
    Geo_Puppet2D puppet1 = Geo_Puppet2D("puppet1", ghostPuppetAssets);
    Geo_Puppet2D puppet2 = Geo_Puppet2D("puppet2", angelPuppetAssets);
    Geo_Puppet2D puppet3 = Geo_Puppet2D("puppet3", demonPuppetAssets);
    // Geo_Puppet3D puppet = Geo_Puppet3D("puppet3D", "puppetModelPath"); // TODO: Test Puppet3D Implementation

#ifdef TOPL_ENABLE_PHYSICS
    Phys_Connector anchors[3]; // Phys_Connector anchor1, anchor2, anchor3;
    Vec3f anchorOffs[3]; // Vec3f anchorOffs[0] = VEC_3F_ZERO, anchorOffs[1] = VEC_3F_ZERO, anchorOffs[2] = VEC_3F_ZERO;
#endif

    Geo_Quad3D floorMesh = Geo_Quad3D();
    Geo_Actor floor = Geo_Actor(&floorMesh);
    Geo_QuadCone roofMesh = Geo_QuadCone();
    Geo_Actor roof = Geo_Actor(&roofMesh);
    Geo_Circle3D pillarMeshes[4] = { Geo_Circle3D(PILLAR_SIZE), Geo_Circle3D(PILLAR_SIZE), Geo_Circle3D(PILLAR_SIZE), Geo_Circle3D(PILLAR_SIZE) };
    Geo_Actor pillars[4] = { Geo_Actor(&pillarMeshes[0]), Geo_Actor(&pillarMeshes[1]), Geo_Actor(&pillarMeshes[2]), Geo_Actor(&pillarMeshes[3]) };
    std::string modelPath = std::string(std::string(MODELS_DIR) + "SpinTop.obj");
    Geo_Model3D models[5] = { Geo_Model3D("model1", modelPath), Geo_Model3D("model2", modelPath), Geo_Model3D("model3", modelPath), Geo_Model3D("model4", modelPath), Geo_Model3D("model5", modelPath) };
#ifdef RASTERON_H
    Img_Base floorTex = Img_Base(checkeredImgOp({ 1024, 1024 }, { 20, 20, 0xAAEE3333, 0xAA3333EE }));
    Img_Base roofTex =  Img_Base(linedImgOp({ 1024, 1024 }, 0xAAFFFF00, color_invert(0xAA00FF00), 10, 0.0));
    Img_Base pillarTex[4] = { 
        Img_Base(gradientImgOp({ 1024, 1024 }, SIDE_Top, 0xAAFF0000, color_invert(0xAAFF0000))),
        Img_Base(gradientImgOp({ 1024, 1024 }, SIDE_Top, 0xAA00FF00, color_invert(0xAA00FF00))),
        Img_Base(gradientImgOp({ 1024, 1024 }, SIDE_Top, 0xAA0000FF, color_invert(0xAA0000FF))),
        Img_Base(gradientImgOp({ 1024, 1024 }, SIDE_Top, 0xAAEEEEEE, color_invert(0xAAEEEEEE)))
    };
    // Img_Base modelTexs[5];
#endif
private:
    Topl_Scene scene2D, scene3D;
} *_DEMO;