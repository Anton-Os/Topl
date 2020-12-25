struct Box {
    float width;
    float height;
    float depth;
};

class Geo_Rect3D : public Geo_RenderObj {
public:
	Geo_Rect3D() : Geo_RenderObj() {}
    Geo_Rect3D(float sideLength) : Geo_RenderObj() {
        mVCount = 8; // Box has 8 vertices
        mICount = 36; // Box has 36 indices
        mBox.width = sideLength;
        mBox.height = sideLength;
        mBox.depth = sideLength;

        mVData = genVertices();
        mIData = genIndices();
		mTData = genTexCoords();
    }
    Geo_Rect3D(float width, float height, float depth) : Geo_RenderObj() {
        mVCount = 8; // Box has 8 vertices
        mICount = 36; // Box has 36 indices
        mBox.width = width;
        mBox.height = height;
        mBox.depth = depth;

        mVData = genVertices();
        mIData = genIndices();
		mTData = genTexCoords();
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
