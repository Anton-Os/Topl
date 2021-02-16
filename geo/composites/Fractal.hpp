#include <cmath>

#include "Geo_Construct.hpp"

class Geo_Fractal : public Geo_Construct {
public:
    Geo_Fractal(
        const std::string& prefix, 
        Topl_SceneManager* sMan, 
        const Geo_Component* initiator,
        unsigned gCount)
    : Geo_Construct(prefix, sMan, geoLink, pow(initiator->getRenderObj()->getVCount(), gCount)){
        fillSceneManager(sMan);
    }

    void updateSceneManager(Topl_SceneManager* sMan) override;
	void move(Topl_SceneManager* sMan, Eigen::Vector3f vec) override;
	void rotate(Topl_SceneManager* sMan, Eigen::Vector3f vec) override;
private:
    void fill(Topl_SceneManager* sMan) override;
};
