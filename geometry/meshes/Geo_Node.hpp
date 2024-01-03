#ifndef GEO_NODE_H

// #ifdef TOPL_ENABLE_MODELS

#include <assimp/scene.h>

#include "Geo_Actor.hpp"

// Node Mesh

#define MESH_SCALE 0.5F

enum MESH_Attribute {
	MESH_Vertex,
	MESH_Index
	// MESH_Bone
};

unsigned getMeshAttribCount(const aiMesh* mesh, MESH_Attribute attrib); // for singular mesh
unsigned getMeshesAttribCount(const std::vector<const aiMesh*>& meshes, MESH_Attribute attrib); // for multiple meshes

class Geo_NodeMesh : public Geo_Mesh {
public:
	Geo_NodeMesh(const aiMesh* mesh) : Geo_Mesh(
		getMeshAttribCount(mesh, MESH_Vertex),
		getMeshAttribCount(mesh, MESH_Index)
	){
		_assimpMeshes = { mesh };
		genVertices(); genIndices();
	}

	Geo_NodeMesh(const std::vector<const aiMesh*>& meshes) : Geo_Mesh(
		getMeshesAttribCount(meshes, MESH_Vertex),
		getMeshesAttribCount(meshes, MESH_Index)
	) {
		_assimpMeshes = std::vector<const aiMesh*>(meshes.begin(), meshes.end());
		genVertices(); genIndices();
	}

	unsigned short getMeshCount() const { return _assimpMeshes.size(); }
private:
	void genVertices() override;
	void genIndices() override;

	std::vector<const aiMesh*> _assimpMeshes;
};

// Node Actor

#define NO_NODE_COUNT 0
#define SINGLE_NODE_COUNT 1

class Geo_NodeActor : public Geo_Actor {
public:
	Geo_NodeActor() : Geo_Actor(){} // Empty Constructor
    Geo_NodeActor(const aiScene* scene, const aiNode* node) : Geo_Actor(){ 
		if (scene != nullptr && node != nullptr) init(scene, node); 
	}
	~Geo_NodeActor() { if(_mesh != nullptr) delete(_mesh);}

	const Geo_NodeMesh* getMesh() const { return _mesh; }
private:
	void init(const aiScene* scene, const aiNode* node);
    const aiScene* _sceneRef = nullptr;
    const aiNode* _nodeRef = nullptr;

	Geo_NodeMesh* _mesh = nullptr;
};

// #endif

#define GEO_NODE_H
#endif