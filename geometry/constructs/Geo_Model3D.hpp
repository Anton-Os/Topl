#ifndef GEO_MODEL_H

#ifdef TOPL_ENABLE_MODELS

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#endif

#include "meshes/Geo_Node.hpp"

#include "Geo_Construct.hpp"

#ifdef TOPL_ENABLE_TEXTURES
unsigned linedSampler_callback(double x, double y){ return (cos(y * 20) < 0.5)? 0xAA00FF00 : 0xAAFF00FF; }
#endif

class Geo_Model3D : public Geo_Construct {
public:
    Geo_Model3D(const std::string& prefix, const std::string& filePath) : Geo_Construct(prefix){ // Non-Configured Constructor
        _filePath = filePath;
        init();
    }
#ifdef TOPL_ENABLE_MODELS
    Geo_Model3D(const std::string& prefix, const std::string& filePath, Topl_Scene* scene) : Geo_Construct(prefix){ // Configured Constructor
        _filePath = filePath;
        init();
        configure(scene);
    }

    ~Geo_Model3D(){
        if(_nodes != nullptr){
            for(unsigned n = 0; n < _nodeCount; n++) delete(*(_nodes + n));
            free(_nodes);
            _nodes = nullptr;
        }
    }

    void init() override {
        Assimp::Importer aiImporter;
        const unsigned aiFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;

        const aiScene* aiScene = aiImporter.ReadFile(_filePath, aiFlags);

        _nodeCount = aiScene->mRootNode->mNumChildren;
        _nodes = (Geo_Node**)malloc(_nodeCount * sizeof(Geo_Node*));
        for (unsigned n = 0; n < _nodeCount; n++) // iterates through all nodes
            *(_nodes + n) = new Geo_Node(aiScene, aiScene->mRootNode->mChildren[n]);

        for (unsigned n = 0; n < _nodeCount; n++) {
            Geo_Node* currentNode = *(_nodes + n);
            if (currentNode->getMesh()->getMeshCount() > NO_NODE_COUNT) {
                _geoNodes.push_back(currentNode);
                _geoActors.push_back(Geo_Actor(*currentNode)); // replace the _geoNodes member
#ifdef TOPL_ENABLE_TEXTURES
                _nodeSampler_map.insert({ _geoNodes[n], Sampler_2D() });
                _nodeSampler_map.at(_geoNodes[n]) = Sampler_Color(RAND_COLOR()); // .setColorImage(RAND_COLOR()); // .setImage(modelImg);
#endif
            }
        }
    }

    void configure(Topl_Scene* scene) override {
        for (unsigned n = 0; n < _geoNodes.size(); n++) {
                scene->addGeometry(getPrefix() + _geoActors[n].getName(), _geoNodes[n]);
#ifdef TOPL_ENABLE_TEXTURES
                scene->addTexture(getPrefix() + _geoActors[n].getName(), &_nodeSampler_map.at(_geoNodes[n]));
#endif
        }
    }

    unsigned getActorCount() const { return _geoNodes.size(); }
    // Geo_Actor* getGeoActor(unsigned short a){ return (Geo_Actor*)&_geoNodes[a]; }
	Sampler_2D* getImgAt(unsigned short i){ return (i < _geoNodes.size())? &_nodeSampler_map[_geoNodes[i]] : nullptr; }
    
    void shift(Vec3f vec) { // replace Geo_Construct shift()
		for (std::vector<Geo_Node*>::iterator n = _geoNodes.begin(); n != _geoNodes.end(); n++)
			(*n)->updatePos({ vec });
	}
    void rotateAll(Vec3f angles){ // piecewise rotation of all actors
        for(unsigned g = 0; g < _geoNodes.size(); g++) _geoNodes[g]->updateRot(angles);
    }
protected:
	std::vector<Geo_Node*> _geoNodes; // geometry nodes only
    std::map<const Geo_Node*, Sampler_2D> _nodeSampler_map; // child backgrounds
	// TODO: include animation and other relevant data
private:
    Geo_Node** _nodes = nullptr; // all nodes data
    unsigned _nodeCount = 0; // all nodes count
#else
    void configure(Topl_Scene* scene) override {  }
#endif
    std::string _filePath; // nodes are read from file
};

#define GEO_MODEL_H
#endif
