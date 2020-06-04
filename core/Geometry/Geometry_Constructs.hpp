#include "Geometry.hpp"
#include "Physics.hpp"

#include "Topl_SceneGraph.hpp"

/* This header defines geometry creation mechanisms */

// void createRandShapes(Topl_SceneGraph* sceneGraph);

Eigen::Vector3f updatePosAbs(const Eigen::Vector3f& pos);


// Implement a Geo_Constructs base class that has a name prefix generator

class Geo_RandShapes {
public:
    Geo_RandShapes(Topl_SceneGraph* sceneGraph){
        mBox1 = new Geo_Rect2D(0.8f, 0.6f);
        mGeoNode1 = new Topl_GeoNode((Geo_RenderObj*)mBox1);

        mBox2 = new Geo_Rect2D(0.2f, 0.3f);
        mGeoNode2 = new Topl_GeoNode((Geo_RenderObj*)mBox2);
        mGeoNode2->updatePos(Eigen::Vector3f(0.6f, -0.55f, 0.0f));

		// mSphere1 = new Geo_Sphere2D(1.0f, 20);
        // mGeoNode3 = new Topl_GeoNode((Geo_RenderObj*)mSphere1);
        // mGeoNode3->updatePos(Eigen::Vector3f(-0.2f, -0.4f, 0.0f));

        fillSceneGraph(sceneGraph);
    }
    ~Geo_RandShapes(){
        delete mBox1;
        delete mGeoNode1;
        delete mBox2;
        delete mGeoNode2;
        // delete mSphere1;
        // delete mGeoNode3;
    }
    void updateSceneGraph(Topl_SceneGraph* Topl_SceneGraph, Timer_EventRatio event); // Should probably make private
private:
    void fillSceneGraph(Topl_SceneGraph* sceneGraph);
    
    Geo_Rect2D* mBox1;
    Geo_Rect2D* mBox2;
	Geo_Sphere2D* mSphere1;
    Topl_GeoNode* mGeoNode1;
    Topl_GeoNode* mGeoNode2;
    Topl_GeoNode* mGeoNode3;
    Physics_MoveAbs mUpMovement = Physics_MoveAbs(&updatePosAbs);
};

void createQuadTree(Topl_SceneGraph* sceneGraph, unsigned divs);
