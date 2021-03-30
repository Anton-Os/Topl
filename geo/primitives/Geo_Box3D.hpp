#include "Geometry.hpp"

struct Box {
    float width;
    float height;
    float depth;
};

class Geo_Box3D : public Geo_RenderObj {
public:
    Geo_Box3D(float sideLength) 
    : Geo_RenderObj(8, 36) {
        mBox.width = sideLength;
        mBox.height = sideLength;
        mBox.depth = sideLength;

        fillRenderObject();
    }
    Geo_Box3D(float width, float height, float depth) 
    : Geo_RenderObj(8, 36) {
        mBox.width = width;
        mBox.height = height;
        mBox.depth = depth;

        fillRenderObject();
    }

	float getWidth() const { return mBox.width; }
	float getHeight() const { return mBox.height; }
    float getDepth() const { return mBox.depth; }
private:
    void genVertices(Eigen::Vector3f* data) override;
	void genTexCoords(Eigen::Vector2f* data) override;
    void genIndices(unsigned* data) override;
    Box mBox;
};
