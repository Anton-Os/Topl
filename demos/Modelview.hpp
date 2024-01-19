#include "constructs/Geo_Puppet.hpp"
#include "constructs/Geo_Model3D.hpp"

#include "Flat_Shader.hpp"
#include "Textured_Shader.hpp"
#include "Beams_Shader.hpp"

#include "Topl_Program.hpp"

struct Modelview_Demo : public Topl_Program {
    Modelview_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Modelview", backend){
        // flatVShader.setMode(2);
        // flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
        // texVShader.setMode(0);
        // texPipeline = Topl_Factory::genPipeline(backend, &texVShader, &texPShader);
        beamsVShader.setMode(0);
        beamsPipeline = Topl_Factory::genPipeline(backend, &beamsVShader, &beamsPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Puppet2D puppet = Geo_Puppet2D("puppet"); // TODO: Test Puppet2D Implementation
    // Geo_Puppet3D puppet = Geo_Puppet3D("puppet"); // TODO: Test Puppet3D Implementation

    Geo_Model3D model = Geo_Model3D("model", std::string(std::string(MODELS_DIR) + "SpinTop.glb"));
private:
    Topl_Scene scene;

    Topl_Pipeline* beamsPipeline; // texPipeline // flatPipeline;
    // Flat_VertexShader_GL4 flatVShader; Flat_PixelShader_GL4 flatPShader;
    // Textured_VertexShader_GL4 texVShader; Textured_PixelShader_GL4 texPShader;
    Beams_VertexShader_GL4 beamsVShader; Beams_PixelShader_GL4 beamsPShader;
} *_instance;