#include "meshes/Geo_Orboid.hpp"
#include "constructs/Geo_Chain.hpp"
#include "constructs/Geo_Grid.hpp"

#include "program/Topl_Program.hpp"

#define GENESIS_SIZE 1.0F
#define GENESIS_COUNT 5
#define GENESIS_ROT 0.0000002F
#define GENESIS_PULSE 0.0000003F
#define GENESIS_NAME "genesis" + std::to_string(rand() % 9999)

struct Genesis_Demo : public Topl_Program {
    Genesis_Demo(const char* execPath) : Topl_Program(execPath, "Genesis"){}
    Genesis_Demo(const char* execPath, BACKEND_Target backend) : Topl_Program(execPath, "Genesis", backend){}

    void init() override;
    void loop(double frameTime) override;
private:
    void onAnyKey(keyboard_t key);
    void onOverlayUpdate(PROGRAM_Menu menu, unsigned short paneIndex) override;

    Topl_Scene scene = PROGRAM_SCENE;

    Geo_Mesh orbs[9] = {
        Geo_Orb(GENESIS_SIZE), 
        Geo_Orb({ GENESIS_SIZE, CIRCLE_SEGMENTS, 10 }),
        Geo_HexOrb(GENESIS_SIZE),
        Geo_Orb({ GENESIS_SIZE, 6, 3 }),
        Geo_OctOrb(GENESIS_SIZE), 
        Geo_Orb({ GENESIS_SIZE, 8, 4 }),
        Geo_DecOrb(GENESIS_SIZE), 
        Geo_Orb({ GENESIS_SIZE, 10, 5 }),
        Geo_TrigOrb(GENESIS_SIZE),
    };

    std::vector<Geo_Chain> chains;
    std::vector<Geo_Grid> grids;
    // std::vector<Genesis_Construct> webs;
} *Genesis;