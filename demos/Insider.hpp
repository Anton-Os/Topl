#include "meshes/Geo_Surface.hpp"
#include "meshes/Geo_Cone.hpp"
#include "meshes/Geo_Parametric.hpp"

#include "Textured_Shader.hpp"

#include "Topl_Program.hpp"

struct Insider_Demo : public Topl_Program {
    Insider_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Insider", backend){
        texPipeline = Topl_Factory::genPipeline(backend, &texVShader, &texPShader);
    }

    void init() override;
    void loop(double frameTime) override;

    Geo_Quad2D floor;
    Geo_Quad2D walls[4];
    Geo_CircleCone cones[4];
    Geo_ParametricSet meshes[3] = {
        Geo_ParametricSet({{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }, }), // basic constructor
        Geo_ParametricSet({{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }, }, { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, }), // paths constuctor
        Geo_ParametricSet({{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f }, }, 0.8F, 5) // insertion constructor
    };

    Geo_Actor floorActor = Geo_Actor((Geo_Mesh*)&floor);
    Geo_Actor wallActors[4] = { 
        Geo_Actor((Geo_Mesh*)&walls[0]), 
        Geo_Actor((Geo_Mesh*)&walls[1]), 
        Geo_Actor((Geo_Mesh*)&walls[2]), 
        Geo_Actor((Geo_Mesh*)&walls[3]), 
    };
    Geo_Actor coneActors[4] = { 
        Geo_Actor((Geo_Mesh*)&cones[0]), 
        Geo_Actor((Geo_Mesh*)&cones[1]), 
        Geo_Actor((Geo_Mesh*)&cones[2]), 
        Geo_Actor((Geo_Mesh*)&cones[3]), 
    };
    Geo_Actor meshActors[3] = {
        Geo_Actor((Geo_Mesh*)&meshes[0]),
        Geo_Actor((Geo_Mesh*)&meshes[1]),
        Geo_Actor((Geo_Mesh*)&meshes[2]),
    };

    Img_Base floorTexture;
    Img_Base wallTextures[4];
    Img_Base coneTextures[4];

private:
    Topl_Scene scene;
    Topl_Scene objScene;

    Topl_Pipeline* texPipeline;
    Textured_VertexShader_GL4 texVShader; Textured_PixelShader_GL4 texPShader;
} *_instance;