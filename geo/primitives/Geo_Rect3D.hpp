#include "Geometry.hpp"

struct Box {
    float width;
    float height;
    float depth;
};

class Geo_Rect3D : public Geo_RenderObj {
public:
    Geo_Rect3D(float sideLength) 
    : Geo_RenderObj(8, 36) {
        mBox.width = sideLength;
        mBox.height = sideLength;
        mBox.depth = sideLength;

        fillRenderObject();
    }
    Geo_Rect3D(float width, float height, float depth) 
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
    Eigen::Vector3f* genVertices() override;
	Eigen::Vector2f* genTexCoords() override;
    unsigned* genIndices() override;
    Box mBox;
};
