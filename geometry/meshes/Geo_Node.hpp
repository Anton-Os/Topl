#ifndef Geo_Node_H

#include <assimp/scene.h>

#include "Geo_Actor.hpp"

// Node Mesh

#define MESH_SCALE 0.5F

enum MESH_Attribute {
	MESH_Vertex,
	MESH_Index
	// MESH_Bone
};

#ifdef __cplusplus
extern "C" {
#endif

DllExport unsigned getMeshAttribCount(const aiMesh* mesh, MESH_Attribute attrib); // for singular mesh
DllExport unsigned getMeshesAttribCount(const std::vector<const aiMesh*>& meshes, MESH_Attribute attrib); // for multiple meshes

// DllExport Geo_Vertex* genModel_vertices(std::string modelPath, unsigned short meshIndex); // TODO: Include functions that are bindable for vertex generation
// DllExport unsigned* genModel_indices(std::string modelPath, unsigned short meshIndex); // TODO: Include functions that are bindable for index generation

#ifdef __cplusplus

class Geo_NodeMesh : public Geo_Mesh {
public:
	DllExport Geo_NodeMesh(const aiMesh* mesh) : Geo_Mesh(
		getMeshAttribCount(mesh, MESH_Vertex),
		getMeshAttribCount(mesh, MESH_Index)
	){
		_assimpMeshes = { mesh };
		init();
	}

	DllExport Geo_NodeMesh(const std::vector<const aiMesh*>& meshes) : Geo_Mesh(
		getMeshesAttribCount(meshes, MESH_Vertex),
		getMeshesAttribCount(meshes, MESH_Index)
	) {
		_assimpMeshes = std::vector<const aiMesh*>(meshes.begin(), meshes.end());
		init();
	}

	DllExport unsigned short getMeshCount() const { return _assimpMeshes.size(); }
private:
	DllExport void init();

	std::vector<const aiMesh*> _assimpMeshes;
};

// Node Actor

#define NO_NODE_COUNT 0
#define SINGLE_NODE_COUNT 1

class Geo_Node : public Geo_Actor {
public:
	DllExport Geo_Node() : Geo_Actor(){} // Empty Constructor
    DllExport Geo_Node(const aiScene* scene, const aiNode* node) : Geo_Actor(){ 
		if (scene != nullptr && node != nullptr) init(scene, node); 
	}
	~Geo_Node() { if(_mesh != nullptr) delete(_mesh);}

	DllExport const Geo_NodeMesh* getMesh() const { return _mesh; }
private:
	DllExport void init(const aiScene* scene, const aiNode* node);
    const aiScene* _sceneRef = nullptr;
    const aiNode* _nodeRef = nullptr;

	Geo_NodeMesh* _mesh = nullptr;
};

#endif

#ifdef __cplusplus
}
#endif

#define Geo_Node_H
#endif