#include "meshes/Geo_Orboid.hpp"

#include "program/Topl_Program.hpp"

#define MOLECULAR_CONSTRUCTS 12
#define MOLECULAR_LENGTH 20.0F
#define MOLECULAR_FORCE 2000.0F
#define MOLECULAR_SIZE 0.05F

#include "Molecular_Construct.h"

struct Molecular_Demo : public Topl_Program {
    Molecular_Demo(const char* execPath) : Topl_Program(execPath, "Molecular"){}
    Molecular_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Molecular", backend){}

    void init() override;
    void loop(double frameTime) override;

    Molecular_Construct constructs[3][MOLECULAR_CONSTRUCTS] = {
        { Molecular_Construct(2), Molecular_Construct(3), Molecular_Construct(4),
          Molecular_Construct(5), Molecular_Construct(6), Molecular_Construct(7),
          Molecular_Construct(8), Molecular_Construct(9), Molecular_Construct(10),
          Molecular_Construct(11), Molecular_Construct(12), Molecular_Construct(16) },
        { Molecular_Construct(2), Molecular_Construct(3), Molecular_Construct(4),
          Molecular_Construct(5), Molecular_Construct(6), Molecular_Construct(7),
          Molecular_Construct(8), Molecular_Construct(9), Molecular_Construct(10),
          Molecular_Construct(11), Molecular_Construct(12), Molecular_Construct(16) },
        { Molecular_Construct(2), Molecular_Construct(3), Molecular_Construct(4),
          Molecular_Construct(5), Molecular_Construct(6), Molecular_Construct(7),
          Molecular_Construct(8), Molecular_Construct(9), Molecular_Construct(10),
          Molecular_Construct(11), Molecular_Construct(12), Molecular_Construct(16) }
    };

    Phys_Connector construct_links[MOLECULAR_CONSTRUCTS - 1];
private:
    void onAnyKey(keyboard_t key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex);

    Topl_Scene scene = PROGRAM_SCENE;
    Topl_Light skyLight = BEAMS_LAMP_LIGHT, flashLight = BEAMS_SKY_LIGHT, lampLight = BEAMS_FLASH_LIGHT;
} *Molecular;
