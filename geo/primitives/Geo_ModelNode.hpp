#include "Geometry.hpp"

#include "FileIO.hpp"
// #include "File3D.hpp"

class Geo_ModelNode : public Geo_RenderObj {
public:
    Geo_ModelNode() : Geo_RenderObj(1, 1){  } // for testing

	void log(const char* fileName); // prints contents for debugging
private:

    void genVertices(Eigen::Vector3f* data) override;
	void genNormals(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
};