#ifndef GEO_MODEL_H

#ifdef TOPL_ENABLE_MODELS

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#endif

#include "meshes/Geo_Node.hpp"

#include "Geo_Construct.hpp"

#ifdef RASTERON_H
unsigned linedImg_callback(double x, double y){ return (cos(y * 20) < 0.5)? 0xAA00FF00 : 0xAAFF00FF; }
#endif

class Geo_Model3D : public Geo_Construct {
public:
    Geo_Model3D(const std::string& prefix, const std::string& filePath) : Geo_Construct(prefix){ // Non-Configured Constructor
        _filePath = filePath;
    }
#ifdef TOPL_ENABLE_MODELS
    Geo_Model3D(const std::string& prefix, const std::string& filePath, Topl_Scene* scene) : Geo_Construct(prefix){ // Configured Constructor
        _filePath = filePath;
        configure(scene);
    }

    ~Geo_Model3D(){
        if(_nodes != nullptr){
            for(unsigned n = 0; n < _nodeCount; n++) delete(*(_nodes + n));
            free(_nodes);
            _nodes = nullptr;
        }
    }

	void shift(Vec3f vec) { // replace Geo_Construct shift()
		for (std::vector<Geo_NodeActor*>::iterator n = _geoNodes.begin(); n != _geoNodes.end(); n++)
			(*n)->updatePos({ vec });
	}
    void rotateAll(Vec3f angles){ // piecewise rotation of all actors
        for(unsigned g = 0; g < _geoNodes.size(); g++) _geoNodes[g]->updateRot(angles);
    }
    void configure(Topl_Scene* scene) override {
        Assimp::Importer aiImporter;
        const unsigned aiFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;

        const aiScene* aiScene = aiImporter.ReadFile(_filePath, aiFlags);

        _nodeCount = aiScene->mRootNode->mNumChildren;
        _nodes = (Geo_NodeActor**)malloc(_nodeCount * sizeof(Geo_NodeActor*));
        for (unsigned n = 0; n < _nodeCount; n++) // iterates through all nodes
            *(_nodes + n) = new Geo_NodeActor(aiScene, aiScene->mRootNode->mChildren[n]);

        for (unsigned n = 0; n < _nodeCount; n++) {
            Geo_NodeActor* currentNode = *(_nodes + n);
            if (currentNode->getMesh()->getMeshCount() > NO_NODE_COUNT) {
                _geoNodes.push_back(currentNode);
                // _geoActors.push_back(Geo_Actor(*currentNode)); // replace the _geoNodes member
                scene->addGeometry(getPrefix() + currentNode->getName(), currentNode);
#ifdef RASTERON_H
                _nodeImg_map.insert({ currentNode, Img_Base() });
                // Rasteron_Image* modelImg = mapImgOp({1024, 1024}, linedImg_callback);
                _nodeImg_map.at(currentNode).setColorImage(RAND_COLOR()); // .setImage(modelImg);
                scene->addTexture(getPrefix() + currentNode->getName(), &_nodeImg_map.at(currentNode));
                // dealloc_image(modelImg);
#endif
            }
        }
    }
protected:
	std::vector<Geo_NodeActor*> _geoNodes; // geometry nodes only
    std::map<const Geo_NodeActor*, Img_Base> _nodeImg_map; // child backgrounds
	// TODO: include animation and other relevant data
private:
    Geo_NodeActor** _nodes = nullptr; // all nodes data
    unsigned _nodeCount = 0; // all nodes count
#else
    void configure(Topl_Scene* scene) override {  }
#endif
    std::string _filePath; // nodes are read from file
};

#define GEO_MODEL_H
#endif