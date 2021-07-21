#include "Geometry.hpp"

// #include "FileIO.hpp"
#include "File3D.hpp"

class Geo_Model3D : public Geo_RenderObj {
public:
    Geo_Model3D(){  } // for testing
    // Model Constructor
    Geo_Model3D(const File3D_Node* node)
    : Geo_RenderObj(node->getVertexCount(), node->getIndexCount()){
		mNode = node;
		
		fillRenderObject();
    }

	void log(const char* fileName); // prints contents for debugging
private:
	friend class File3D_Node;
	const File3D_Node* mNode;

    void genVertices(Eigen::Vector3f* data) override;
	void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
};
