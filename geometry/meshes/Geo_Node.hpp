#define ASSIMP_FOUND

#include <assimp/scene.h>

#include "Geo_Actor.hpp"

#define MESH_SCALE 0.1

enum MESH_Attribute {
	MESH_Vertex,
	MESH_Index
	// MESH_Bone
};

unsigned getMeshAttribCount(const aiMesh* mesh, MESH_Attribute attrib); // for singular mesh
unsigned getMeshesAttribCount(const std::vector<const aiMesh*>& meshes, MESH_Attribute attrib); // for multiple meshes

class Geo_Node : public Geo_Mesh {
public:
	Geo_Node(const aiMesh* mesh)
	: Geo_Node(
		getMeshAttribCount(mesh, MESH_Vertex),
		getMeshAttribCount(mesh, MESH_Index)
	){
		_assimpMeshes = { mesh };
		genVertices(); genIndices();
	}

	Geo_Node(const std::vector<const aiMesh*>& meshes)
	: Geo_Node(
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

class Geo_NodeActor : public Geo_Actor {
public:
    Geo_NodeActor(const aiScene* scene, const aiNode* node);
	~Geo_NodeActor() { if(_mesh != nullptr) delete(_mesh);}

	const Geo_Node* getMesh() const { return _mesh; }
private:
    const aiScene* _scene = nullptr;
    const aiNode* _node = nullptr;

	Geo_Node* _mesh = nullptr;
};

#endif