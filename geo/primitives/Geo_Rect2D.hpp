#include "Geometry.hpp"

struct Rect {
    float width;
    float height;
};

class Geo_Rect2D : public Geo_RenderObj {
public:
    // Empty constructor
    Geo_Rect2D() : Geo_RenderObj(){}
    // Box constructor
    Geo_Rect2D(float l) 
    : Geo_RenderObj(4, 6) // 4 vertices and 6 indices
    {
        mRect.width = l;
        mRect.height = l;

        fillRenderObject();
    }
    // Rectangle constructor
    Geo_Rect2D(float w, float h) 
    : Geo_RenderObj(4, 6) // 4 vertices and 6 indices
    {
        mRect.width = w;
        mRect.height = h;

        fillRenderObject();
    }

	float getWidth() const { return mRect.width; }
	float getHeight() const { return mRect.height; }
private:
    Eigen::Vector3f* genVertices() override;
	Eigen::Vector2f* genTexCoords() override;
    unsigned* genIndices() override;
    Rect mRect;
};
