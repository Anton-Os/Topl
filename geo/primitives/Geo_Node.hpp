#include <assimp/scene.h>

#include "Geometry.hpp"

#include "Geo_Actor.hpp"

class Geo_Mesh : public Geo_RenderObj {
public:
    Geo_Mesh(const aiMesh* mesh) : Geo_RenderObj(mesh->mNumVertices, getIndexCount(mesh)){  }

private:
    static unsigned getIndexCount(const aiMesh* mesh);

    void genVertices(Eigen::Vector3f* data) override;
	void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;

    unsigned _boneCount;
};

class Geo_Node : public Geo_Actor {
public:
    Geo_Node(const aiNode* node) : Geo_Actor(){  } // empty constructor for testing

private:

    std::string parentName; // if root node then this will remain blank
};
