#include <assimp/scene.h>

#define MESH_SCALE 0.1

#include "ShapesGen.hpp"

#include "Geo_Actor.hpp"

class Geo_Mesh : public Geo_RenderObj {
public:
	Geo_Mesh() : Geo_RenderObj() {}
	Geo_Mesh(const aiMesh* mesh) : Geo_RenderObj(mesh->mNumVertices, getIndexCount(mesh)) {
		_assimpMeshRef = mesh;
		fillRenderObj();
	}
	virtual ~Geo_Mesh() {}

	void cloneMesh(const Geo_Mesh* mesh) {
		clone((Geo_RenderObj*)mesh);
		_assimpMeshRef = mesh->getAssimpMeshRef();
	}
	const aiMesh* getAssimpMeshRef() const { return _assimpMeshRef; }
private:
	static unsigned getIndexCount(const aiMesh* mesh); // gets indices by iterating through faces

	void genPos(Vec3f* data) override;
	void genNormals(Vec3f* data) override;
	void genTexCoords(Vec2f* data) override;
	void genIndices(unsigned* data) override;

	const aiMesh* _assimpMeshRef = nullptr;
	// Add extra mesh parameters here!
};

class Geo_Node : public Geo_Actor {
public:
    Geo_Node(const aiScene* scene, const aiNode* node);
	~Geo_Node() { if(_mesh != nullptr) delete(_mesh); }

	unsigned getMeshCount() const { return _meshCount; }
	const Geo_Mesh* getMesh() const { return _mesh; }
private:
    const aiScene* _scene = nullptr;
    const aiNode* _node = nullptr;

	unsigned _meshCount = 0;
	Geo_Mesh* _mesh = new Geo_Mesh();
};
