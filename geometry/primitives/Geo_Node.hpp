#include <assimp/scene.h>

#include "Geometry.hpp"

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

	void genPos(Eigen::Vector3f* data) override;
	void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
	void genIndices(unsigned* data) override;

	const aiMesh* _assimpMeshRef = nullptr;
	// Add extra mesh parameters here!
};

class Geo_Node : public Geo_Actor {
public:
    Geo_Node(const aiScene* scene, const aiNode* node) : 
    Geo_Actor(){
		if (scene != nullptr && node != nullptr) {
			setName(std::string(node->mName.C_Str()));
			_scene = scene;
			_node = node;

			_meshCount = node->mNumMeshes;
			if (_meshCount == 1) {
				Geo_Mesh mesh = Geo_Mesh(*(_scene->mMeshes + *(_node->mMeshes)));
				_mesh->cloneMesh(&mesh);
				setRenderObj((Geo_RenderObj*)_mesh);
			}
			else if (_meshCount > 1) {
				puts("Multiple meshes detected!");
				// TODO: implement code to fuse multiple meshes together!

				Geo_Mesh mesh = Geo_Mesh(*(_scene->mMeshes + *(_node->mMeshes)));
				_mesh->cloneMesh(&mesh);
				setRenderObj((Geo_RenderObj*)_mesh);
			}
			else puts("No meshes detected!");
		}
    }
	~Geo_Node() { if(_mesh != nullptr) delete(_mesh); }

	unsigned getMeshCount() const { return _meshCount; }
	const Geo_Mesh* getMesh() const { return _mesh; }
private:
    const aiScene* _scene = nullptr;
    const aiNode* _node = nullptr;

	unsigned _meshCount = 0;
	Geo_Mesh* _mesh = new Geo_Mesh();
};
