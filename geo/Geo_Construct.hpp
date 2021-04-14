#ifndef GEO_CONSTRUCT_H

#include <initializer_list>
#include <string>

#include "Geometry.hpp"
#include "Timer.hpp"

#include "Topl_Scene.hpp"

// Geometry wrapper class that can manage its states

#define FIRST_UPDATE_NUM 0

typedef std::pair<std::string, Geo_Component*> geoName_pair;

// TODO: Node data must be changed to geo data
class Geo_Construct {
public:
    // Fixed items constructor
    Geo_Construct(const std::string& prefix, Topl_Scene* scene, std::initializer_list<Geo_RenderObj*> renderObjs) {
		mPrefix = prefix;
        mGeoData = (Geo_Component**)malloc(renderObjs.size() * sizeof(Geo_Component*));
        for(std::initializer_list<Geo_RenderObj*>::iterator currentRenderObj = renderObjs.begin(); currentRenderObj < renderObjs.end(); currentRenderObj++){
            *(mGeoData + mGeoCount) = new Geo_Component(*(currentRenderObj));
            mGeoCount++;
        }
	}
    // Duplicate items constructor
    Geo_Construct(const std::string& prefix, Topl_Scene* scene, const Geo_Component* geoc, unsigned count) {
		mPrefix = prefix;
        mGeoCount = count;
        mGeoData = (Geo_Component**)malloc(count * sizeof(Geo_Component));

        for(unsigned g = 0; g < count; g++) *(mGeoData + g) = new Geo_Component(*geoc);
	}

	~Geo_Construct() { 
        if(mGeoData != nullptr) { 
            for (unsigned g = 0; g < mGeoCount; g++)
                delete *(mGeoData + g);
            free(mGeoData);
        }
    }

    // Should be called in the derived class constructor body
    void fillSceneManager(Topl_Scene* scene){
        fill(scene);

        // Code that fills in scene
        if(mNamedGeos.size() != 0 && mGeoData != nullptr) {
            for(std::vector<geoName_pair>::iterator currentGeo = mNamedGeos.begin();
                currentGeo < mNamedGeos.end(); currentGeo++)
                scene->addGeometry(currentGeo->first, currentGeo->second);
        }
    }

    std::string getPrefix(){ return mPrefix + "_"; }
	virtual void updateSceneManager(Topl_Scene* scene) = 0;
    void move(Topl_Scene* scene, Eigen::Vector3f vec){
        for(unsigned g = 0; g < mGeoCount; g++) scene->addForce((*(mGeoData + g))->getName(), vec);
    }
    void rotate(Topl_Scene* scene, Eigen::Vector2f angles){
        for(unsigned g = 0; g < mGeoCount; g++) (*(mGeoData + g))->updateRot(angles);
    }
    /* virtual void move(Topl_Scene* scene, Eigen::Vector3f vec) = 0;
    virtual void rotate(Topl_Scene* scene, Eigen::Vector3f) = 0; */

protected:
	unsigned getGeoCount() const { return mGeoCount; }
    virtual void fill(Topl_Scene* scene) = 0; // Job is to fill the mNamedGeos structure
    Geo_Component* getNextGeo(){
        if(mCurrentGeoOffset <= mGeoCount){
            mCurrentGeoOffset++;   
            return *(mGeoData + mCurrentGeoOffset - 1); // To increment offset above in one line
        } else return nullptr;
    }

    unsigned mUpdateCount = FIRST_UPDATE_NUM; // Probably needs to be private and a getter method
	std::vector<geoName_pair> mNamedGeos;
private:
    std::string mPrefix;
	unsigned mGeoCount = 0;
    unsigned mCurrentGeoOffset = 0;
	Geo_Component** mGeoData = nullptr;
}; // Needs work, could use more abstraction

#define GEO_CONSTRUCT_H
#endif