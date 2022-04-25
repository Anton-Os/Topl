#include <vector>

#include <assimp/scene.h>

#include "ShapesGen.hpp"

#include "Geo_Actor.hpp"

#define MESH_SCALE 0.1

unsigned getIndexCount(const aiMesh* mesh); // gets indices by iterating through faces

class Geo_Mesh : public Geo_RenderObj {
public:
	Geo_Mesh() : Geo_RenderObj() {}
	Geo_Mesh(const aiMesh* mesh) : Geo_RenderObj(mesh->mNumVertices, ::getIndexCount(mesh)) {
		_assimpMeshRef = mesh;
		fillRenderObj();
	}
	Geo_Mesh(const std::vector<const aiMesh*>& meshes) : Geo_RenderObj(/* getVertexCount(meshes), getIndexCount(meshes) */) {
		_assimpMeshRefs = std::vector<const aiMesh*>(meshes);
		fillRenderObj();
	}
	virtual ~Geo_Mesh() {}

	const aiMesh* getAssimpMeshRef() const { return _assimpMeshRef; }
private:
	void genPos(Vec3f* data) override;
	void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
	void genIndices(unsigned* data) override;

	const aiMesh* _assimpMeshRef = nullptr; // singular mesh
	std::vector<const aiMesh*> _assimpMeshRefs; // multiple meshes
};

class Geo_Node : public Geo_Actor {
public:
    Geo_Node(const aiScene* scene, const aiNode* node);
	~Geo_Node() { if(_mesh != nullptr) delete(_mesh);}

	unsigned getMeshCount() const { return _meshCount; }
	const Geo_Mesh* getMesh() const { return _mesh; }
private:
    const aiScene* _scene = nullptr;
    const aiNode* _node = nullptr;

	unsigned _meshCount = 0;
	Geo_Mesh* _mesh = nullptr;
};
