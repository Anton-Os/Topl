#include <cmath>

#include "Geo_Construct.hpp"

class Geo_Fractal : public Geo_Construct {
public:
    Geo_Fractal(
        const std::string& prefix, 
        Topl_Scene* scene, 
        const Geo_Component* initiator,
        unsigned gCount)
    : Geo_Construct(prefix, scene, geoLink, pow(initiator->getRenderObj()->getVertexCount(), gCount)){
        fillSceneManager(scene);
    }

    void updateSceneManager(Topl_Scene* scene) override;
	void move(Topl_Scene* scene, Eigen::Vector3f vec) override;
	void rotate(Topl_Scene* scene, Eigen::Vector3f vec) override;
private:
    void fill(Topl_Scene* scene) override;
};
