#include "Geo_Constructs.hpp"

void Geo_Character1::fill(Topl_SceneManager* sMan) {

	FileImage headImg, torsoImg, leftArmImg, rightArmImg, leftLegImg, rightLegImg;
	float width, height; // I will use topRight_Pt as reference for now, update Rasteron for macros and width/height

	rstnLoadFromFile("C:\\AntonDocs\\Design\\UrkwinArt\\Normguy\\Head.png", &headImg);
	Rasteron_Image* headRstnImg = rstnCreate_ImgBase(&headImg);
	Rasteron_Sprite* headSprite = rstnCreate_Sprite(headRstnImg);

	width = headSprite->bounds.topRight_Pt[0] * 2;
	height = headSprite->bounds.topRight_Pt[1] * 2;
	mHead_rect = new Geo_Rect2D(width, height);
	mHead_gNode = new Topl_GeoNode((Geo_RenderObj*)mHead_rect);
	addGeometry("head", mHead_gNode); // TODO: Fix code above to get this working
	
	rstnDel_Sprite(headSprite);
	rstnDel_Img(headRstnImg);
	rstnDelFromFile(&headImg);

    // Possibly code for adding connectors
}

void Geo_Character1::updateSceneManager(Topl_SceneManager* sMan) {
    // sMan->addForce("sphere1", Eigen::Vector3f(0.1f, 0.0f, 0.0));
    // sMan->addForce("sphere2", Eigen::Vector3f(0.1f, 0.2f, 0.0));
    // sMan->addForce("sphere3", Eigen::Vector3f(-0.1f, -0.08f, 0.0));
    return;
}