#include "meshes/Geo_Orboid.hpp"

#include "Topl_Demo.hpp"

#define MOLECULAR_CONSTRUCTS 9
#define MOLECULAR_LENGTH 1.0F
#define MOLECULAR_PULSE 0.0000005F
#define MOLECULAR_FORCE 1000.0F
#define MOLECULAR_SIZE 0.15F
#define MOLECULAR_SPREAD 2.5F

#include "Molecular_Construct.h"

struct Molecular_Demo : public Topl_Demo {
    Molecular_Demo(const char* execPath) : Topl_Demo(execPath, "Molecular"){}
    Molecular_Demo(const char* execPath, BACKEND_Target backend) : Topl_Demo(execPath, "Molecular", backend){}

    void init() override;
    void loop(double frameTime) override;

    Molecular_Construct constructs[3][MOLECULAR_CONSTRUCTS] = {
        { Molecular_Construct(2), Molecular_Construct(3), Molecular_Construct(4),
          Molecular_Construct(5), Molecular_Construct(6), Molecular_Construct(7),
          Molecular_Construct(8), Molecular_Construct(9), Molecular_Construct(10), },
        { Molecular_Construct(2, 2), Molecular_Construct(3, 2), Molecular_Construct(4, 2),
          Molecular_Construct(5, 2), Molecular_Construct(6, 2), Molecular_Construct(7, 2),
          Molecular_Construct(8, 2), Molecular_Construct(9, 2), Molecular_Construct(10, 2), },
        { Molecular_Construct(2, 4), Molecular_Construct(3, 4), Molecular_Construct(4, 4),
          Molecular_Construct(5, 4), Molecular_Construct(6, 4), Molecular_Construct(7, 4),
          Molecular_Construct(8, 4), Molecular_Construct(9, 4), Molecular_Construct(10, 4), }
    };

    Phys_Connector construct_links[MOLECULAR_CONSTRUCTS - 1];
private:
    void onAnyKey(keyboard_t key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex);

    Topl_Scene scene = PROGRAM_SCENE;
    Topl_Light skyLight = BEAMS_LAMP_LIGHT, flashLight = BEAMS_SKY_LIGHT, lampLight = BEAMS_FLASH_LIGHT;
} *Molecular;
