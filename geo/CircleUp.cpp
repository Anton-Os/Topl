#include "Geometry_Constructs.hpp"

void Geo_CircleUp::fill() {
    Topl_GeoNode* gSphere1 = getNextNode();
    gSphere1->updatePos(Eigen::Vector3f(-0.4, -0.24, 0.0));
	// mNamedNodes.push_back(std::make_pair(std::string("sphere1"), gSphere1));
    addGeometry("sphere1", gSphere1);

    Topl_GeoNode* gSphere2 = getNextNode();
    gSphere1->updatePos(Eigen::Vector3f(-0.4, 0.3, 0.0));
	// mNamedNodes.push_back(std::make_pair(std::string("sphere2"), gSphere2));
    addGeometry("sphere2", gSphere2);

	Topl_GeoNode* gSphere3 = getNextNode();
    gSphere1->updatePos(Eigen::Vector3f(0.0, -0.6, 0.0));
	// mNamedNodes.push_back(std::make_pair(std::string("sphere3"), gSphere3));
    addGeometry("sphere3", gSphere3);
} // Should be a method of the base class

void Geo_CircleUp::updateSceneManager(Topl_SceneManager* sMan) {
    sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
}