#include "Geo_Construct.hpp"

class Geo_Web : public Geo_Construct, public Geo_DynamicSet {
public:
    Geo_Web(
        const std::string& prefix,
        Topl_Scene* scene,
        const Geo_Component* geo,
        std::vector<Eigen::Vector3f> offsets
    ) 
    : Geo_Construct(prefix, scene, geo, offsets.size() + 1),
    Geo_DynamicSet(offsets.size() + 1){ // includes origin point
        savedOffsets = offsets;
        
        fillScene(scene);
    }

    void updateScene(Topl_Scene* scene) override;
private:
    void fill(Topl_Scene* scene) override;

    std::vector<Eigen::Vector3f> savedOffsets;
};