#include "Geo_Tree.hpp"

struct Geo_Chain_Properties {
	Geo_Chain_Properties() { directionVec = Eigen::Vector3f(0.1f, 0.0f, 0.0f); }
    Geo_Chain_Properties(Eigen::Vector3f d) { directionVec = d; }
    
    Eigen::Vector3f directionVec;
};

class Geo_Chain : public Geo_Tree, public Geo_DynamicSet {
public:
    Geo_Chain(
        const std::string& prefix, 
        Topl_Scene* scene, 
        const Geo_Actor* geo, 
        const Geo_Chain_Properties* props,
        unsigned count)
    : Geo_Tree(prefix, scene, geo, count),
    Geo_DynamicSet(count){
		properties = *props;
        origin = Eigen::Vector3f(
            (-1.0 * props->directionVec.x() * count) / 2,
            (-1.0 * props->directionVec.y() * count) / 2,
            (-1.0 * props->directionVec.z() * count) / 2
        );

        fill(scene);
    }
private:
    void fill(Topl_Scene* scene) override;

    Eigen::Vector3f origin; // determines starting position for geometry
    Geo_Chain_Properties properties;
};
