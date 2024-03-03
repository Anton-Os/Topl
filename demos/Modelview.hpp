#include "constructs/Geo_Puppet.hpp"
#include "constructs/Geo_Model3D.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"

#include "Topl_Program.hpp"

struct Modelview_Demo : public Topl_Program {
    Modelview_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Modelview", backend){
        // flatVShader.setMode(2);
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
        // texVShader.setMode(0);
        texPipeline = Topl_Factory::genPipeline(backend, &texVShader, &texPShader);
        // beamsVShader.setMode(0);
        beamsPipeline = Topl_Factory::genPipeline(backend, &beamsVShader, &beamsPShader);
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

    Geo_Model3D model1 = Geo_Model3D("model1", std::string(std::string(MODELS_DIR) + "SpinTop.obj"));
    Geo_Model3D model2 = Geo_Model3D("model2", std::string(std::string(MODELS_DIR) + "SpinTop.obj"));
    Geo_Model3D model3 = Geo_Model3D("model3", std::string(std::string(MODELS_DIR) + "SpinTop.obj"));
private:
    Topl_Scene scene2D, scene3D;

    Topl_Pipeline *flatPipeline, *texPipeline, *beamsPipeline;
    Flat_VertexShader_GL4 flatVShader; Flat_PixelShader_GL4 flatPShader;
    Textured_VertexShader_GL4 texVShader; Textured_PixelShader_GL4 texPShader;
    Beams_VertexShader_GL4 beamsVShader; Beams_PixelShader_GL4 beamsPShader;
} *_instance;