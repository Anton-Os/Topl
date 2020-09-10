#include "Geo_Constructs.hpp"

// TODO: Because chain does not require a unique name, Chain01 or Chain02 works just fine and an Id is all that is needed

struct Chain_Properties {
    const Geo_RenderObj* renderObj = nullptr; // INITIALIZE IN CONSTRUCTOR!!!
    const Phys_Connector* connectors = nullptr; // INITIALIZE IN CONSTRUCTOR!!!
    unsigned linkedItemsCount = 2; // Could be substituted with link count
}

/* TODO: Because linked items count is scalable an initializer list does not cut it for creating the Geo chain object,
make a Geo_Construct version that is scalable and has repetitive inputs */

class Geo_Chain : public Geo_Construct {
public:
    Geo_Chain(const std::string& prefix, Topl_SceneManager* sMan, const Chain_Properties* chain_prop)
    : Geo_Construct() // LOOKUP INITIALIZER LIST DOCUMENTATION
    { fillSceneManager(sMan); }

    void updateSceneManager(Topl_SceneManager* sMan) override;
private:
    void fill(Topl_SceneManager* sMan) override;

    // More properties include here
}