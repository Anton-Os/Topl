#include "constructs/Geo_Puppet.hpp"
#include "constructs/Geo_Model3D.hpp"

#include "Flat_Shader.hpp"

#include "Topl_Program.hpp"

struct Viewer_Demo : public Topl_Program {
    Viewer_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "3DViewer", backend){
        flatPipeline = Topl_Factory::genPipeline(backend, &flatVShader, &flatPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Puppet2D puppet = Geo_Puppet2D("puppet"); // TODO: Test Puppet2D Implementation
    // Geo_Puppet3D puppet = Geo_Puppet3D("puppet"); // TODO: Test Puppet3D Implementation

    Geo_Model3D model = Geo_Model3D("model", std::string(std::string(MODELS_DIR) + "SpinTop.glb"));
private:
    Topl_Scene scene;

    Topl_Pipeline* flatPipeline;
    Flat_VertexShader_GL4 flatVShader; Flat_PixelShader_GL4 flatPShader;
} *_instance;