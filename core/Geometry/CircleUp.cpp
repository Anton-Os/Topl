#include "Geometry_Constructs.hpp"

void Geo_CircleUp::fillSceneManager(Topl_SceneManager* sGraph) {
	mNamedNodes.push_back(std::make_pair(std::string("sphere1"), *(mNodeData + 0)));
	mNamedNodes.push_back(std::make_pair(std::string("sphere2"), *(mNodeData + 1)));
	mNamedNodes.push_back(std::make_pair(std::string("sphere3"), *(mNodeData + 2)));

    // Dormant for now
    for(std::vector<geoName_pair>::iterator currentGeo = mNamedNodes.begin();
    currentGeo < mNamedNodes.end(); currentGeo++)
        sGraph->addGeometry(currentGeo->first, currentGeo->second);
} // Should be a method of the base class

void Geo_CircleUp::updateSceneManager(Topl_SceneManager* sGraph) {
    sGraph->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    sGraph->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    sGraph->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
}