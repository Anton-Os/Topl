#include "Geometry.hpp"

#include "FileIO.hpp"

class Geo_Model3D : public Geo_RenderObj {
public:
    // Model Constructor
    Geo_Model3D() 
    : Geo_RenderObj(1, 1) { // TODO: Use proper values
        // mModelNode = modelNode;
        fillRenderObject();
    }
private:
    void genVertices(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;

};
