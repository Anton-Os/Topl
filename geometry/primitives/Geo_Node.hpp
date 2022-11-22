#include <vector>

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

class Geo_Mesh : public Geo_RenderObj {
public:
	Geo_Mesh() : Geo_RenderObj() {}
	Geo_Mesh(const aiMesh* mesh) 
	: Geo_RenderObj(getMeshAttribCount(mesh, MESH_Vertex), getMeshAttribCount(mesh, MESH_Index)) {
		_assimpMeshes = { mesh };
		init();
	}
	Geo_Mesh(const std::vector<const aiMesh*>& meshes) 
	: Geo_RenderObj(getMeshesAttribCount(meshes, MESH_Vertex), getMeshesAttribCount(meshes, MESH_Index)) {
		_assimpMeshes = std::vector<const aiMesh*>(meshes.begin(), meshes.end());
		init();
	}
	virtual ~Geo_Mesh() {}

	unsigned short getMeshCount() const { return _assimpMeshes.size(); }
private:
	void genPos(Vec3f* data) override;
	void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
	void genIndices(unsigned* data) override;

	std::vector<const aiMesh*> _assimpMeshes; // multiple meshes are combined
};

class Geo_Node : public Geo_Actor {
public:
    Geo_Node(const aiScene* scene, const aiNode* node);
	~Geo_Node() { if(_mesh != nullptr) delete(_mesh);}

	const Geo_Mesh* getMesh() const { return _mesh; }
private:
    const aiScene* _scene = nullptr;
    const aiNode* _node = nullptr;

	Geo_Mesh* _mesh = nullptr;
};
