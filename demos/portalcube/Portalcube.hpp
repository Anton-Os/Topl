#include "Geo_Construct.hpp"
#include "meshes/Geo_Volume.hpp"

#include "program/Topl_Program.hpp"

struct Portalcube_Demo : public Topl_Program {
    Portalcube_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Portalcube", backend){}

    void init() override;
    void loop(double frameTime) override;

    Geo_Quad3D cubeMesh = Geo_Quad3D(1.0);

    Geo_Actor cubeActor = Geo_Actor(&cubeMesh);
#ifdef RASTERON_H
    Sampler_3D cubeTex = Sampler_3D(1024);

    Sampler_2D frontFaceTex = Sampler_Color(RAND_COLOR());
    Sampler_2D backFaceTex = Sampler_Color(RAND_COLOR());
    Sampler_2D topFaceTex = Sampler_Color(RAND_COLOR());
    Sampler_2D bottomFaceTex = Sampler_Color(RAND_COLOR());
    Sampler_2D leftFaceTex = Sampler_Color(RAND_COLOR());
    Sampler_2D rightFaceTex = Sampler_Color(RAND_COLOR());
#endif
private:
    void onAnyKey(char key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Topl_Scene scene = PROGRAM_SCENE;
} *Portalcube;
