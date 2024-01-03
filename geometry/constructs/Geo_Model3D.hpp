#ifndef GEO_MODEL_H

#ifdef TOPL_ENABLE_MODELS

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#endif

#include "meshes/Geo_Node.hpp"

#include "Geo_Construct.hpp"

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

	void move(Vec3f vec) { // replace Geo_Construct move()
		for (std::vector<Geo_NodeActor*>::iterator n = _geoNodes.begin(); n != _geoNodes.end(); n++)
			(*n)->updatePos({ vec });
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
                // TODO: Add actor to Geo_Construct parent class
                _geoNodes.push_back(currentNode);
                scene->addGeometry(getPrefix() + currentNode->getName(), currentNode);
            }
        }
    }
protected:
	std::vector<Geo_NodeActor*> _geoNodes; // geometry nodes only
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