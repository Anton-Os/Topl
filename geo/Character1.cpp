#include "Rasteron.h"

#include "Geometry_Constructs.hpp"

void Geo_Character1::fill() {
    Topl_GeoNode* gHead = getNextNode();
    addGeometry("head", gHead);

    Topl_GeoNode* gTorso = getNextNode();
    addGeometry("torso", gTorso);
    
    Topl_GeoNode* gLeftArm = getNextNode();
    addGeometry("leftArm", gLeftArm);
    
    Topl_GeoNode* gRightArm = getNextNode();
    addGeometry("rightArm", gRightArm);
    
    Topl_GeoNode* gLeftLeg = getNextNode();
    addGeometry("leftLeg", gLeftLeg);
    
    Topl_GeoNode* gRightLeg = getNextNode();
    addGeometry("rightLeg", gRightLeg);
    
	Image headImg, torsoImg, leftArmImg, rightArmImg, leftLegImg, rightLegImg;

	rstnLoad("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Head.png", &headImg);
	Rasteron_Image* headRstnImg = rstnCreate_ImgBase(&headImg);
	// Scene manager should add the texture here
	rstnDel_Img(headRstnImg);
	rstnDel(&headImg);

    // Possibly code for adding connectors
}

void Geo_Character1::updateSceneManager(Topl_SceneManager* sMan) {
    // sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    // sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    // sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
    return;
}