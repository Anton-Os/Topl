#include "meshes/Geo_Orboid.hpp"
#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Grid.hpp"

#include "program/Topl_Program.hpp"

#define GENESIS_SIZE 1.0F
#define GENESIS_COUNT 5
#define GENESIS_NAME "genesis" + std::to_string(rand() % 9999)

struct Genesis_Demo : public Topl_Program {
    Genesis_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Genesis", backend){}

    void init() override;
    void loop(double frameTime) override;
private:
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Topl_Scene scene = PROGRAM_SCENE;

    Geo_Mesh orbs[9] = {
        Geo_Orb(GENESIS_SIZE), Geo_Orb(GENESIS_SIZE * 2),
        Geo_HexOrb(GENESIS_SIZE), Geo_HexOrb(GENESIS_SIZE * 2),
        Geo_OctOrb(GENESIS_SIZE), Geo_OctOrb(GENESIS_SIZE * 2),
        Geo_DecOrb(GENESIS_SIZE), Geo_DecOrb(GENESIS_SIZE * 2),
        Geo_Torus(GENESIS_SIZE)
    };

    std::vector<Geo_Chain> chains;
    std::vector<Geo_Grid> grids;
    // std::vector<Genesis_Construct> webs;
} *Genesis;