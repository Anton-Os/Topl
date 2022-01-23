#include <assimp/scene.h>

#include "Geometry.hpp"

#include "Geo_Actor.hpp"

class Geo_Mesh : public Geo_RenderObj {
public:
    Geo_Mesh(const aiMesh* mesh) : Geo_RenderObj(mesh->mNumVertices, getIndexCount(mesh)){
        _mesh = mesh;
    }

private:
    static unsigned getIndexCount(const aiMesh* mesh); // gets indices by iterating through faces

    void genVertices(Eigen::Vector3f* data) override;
	void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;

    const aiMesh* _mesh = nullptr;
    // unsigned _boneCount;
};

class Geo_Node : public Geo_Actor {
public:
    Geo_Node(const aiScene* scene, const aiNode* node) : 
    Geo_Actor(){
		if (scene != nullptr && node != nullptr)
			init(scene, node);
    }
private:
	void init(const aiScene* scene, const aiNode* node);

    const aiScene* _scene = nullptr;
    const aiNode* _node = nullptr;

    unsigned short _meshCount = 0;
    Geo_Mesh* _meshes;
};
