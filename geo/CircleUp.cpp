#include "Geo_Constructs.hpp"

void Geo_CircleUp::fill(Topl_SceneManager* sMan) {
    Topl_GeoNode* gSphere1 = getNextNode();
    gSphere1->updatePos(Eigen::Vector3f(-0.4, -0.24, 0.0));
    sMan->addGeometry("sphere1", gSphere1);

    Topl_GeoNode* gSphere2 = getNextNode();
    gSphere1->updatePos(Eigen::Vector3f(-0.4, 0.3, 0.0));
    sMan->addGeometry("sphere2", gSphere2);

	Topl_GeoNode* gSphere3 = getNextNode();
    gSphere1->updatePos(Eigen::Vector3f(0.0, -0.6, 0.0));
    sMan->addGeometry("sphere3", gSphere3);
} // Should be a method of the base class

void Geo_CircleUp::updateSceneManager(Topl_SceneManager* sMan) {
	if(mUpdateCount == FIRST_UPDATE_NUM) sMan->addPhysics("sphere1", &mPhysProp1);
    sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));

	if (mUpdateCount == FIRST_UPDATE_NUM) sMan->addPhysics("sphere2", &mPhysProp2);
    sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));

	if (mUpdateCount == FIRST_UPDATE_NUM) sMan->addPhysics("sphere3", &mPhysProp3);
    sMan->addForce("sphere3", Eigen::Vector3f(-0.01f, -0.008f, 0.0));

	mUpdateCount++; // TODO: Make this increment a part of Geo_Construct
}