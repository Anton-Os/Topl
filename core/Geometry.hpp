#include <cstdlib>
#include <vector>
#include <map>

#include <Eigen/Dense>

// Simple Primitive Types

struct Rect {
    float width;
    float height;
};

struct Circle {
    float radius;
    unsigned segments;
};

// Objects that are candidates for Topl_GeoEntity::GeoType
// Topl_BallSprite should be added

class Geo_RenderObj {
public:
    ~Geo_RenderObj(){
        if(mVData != nullptr) free(mVData);
        if(mIData != nullptr) free(mIData);
    }

    unsigned getVCount() const { return mVCount; }
    unsigned getICount() const { return mICount; }

    // const Eigen::Vector3f(const*) vData_refPtr = mVData;
    // const unsigned(const*) iData_refPtr = mIData;
    // Eigen::Vector3f(const*) getVData() const { return mVData; }
    // unsigned(const*) getIData() const { return mIData; }
protected:
    // Eigen::Vector3f vec; // Work plz
    virtual Eigen::Vector3f* genVertices() = 0;
    virtual unsigned* genIndices() = 0;

    const unsigned short mPerVertex = 3; // Elements per vertex, should be configurable later
    unsigned mVCount;
    unsigned mICount;
    Eigen::Vector3f* mVData = nullptr;
    unsigned* mIData = nullptr;
};

// Override the virtual functions above
class Geo_Rect2D : Geo_RenderObj {
public:
    Geo_Rect2D(float width, float height){
        mVCount = 4; // Rectangle has 4 vertices
        mICount = 6; // Rectangle has 6 indices
        mRect.width = width;
        mRect.height = height;

        mVData = genVertices();
        mIData = genIndices();
    }
private:
    Eigen::Vector3f* genVertices() override;
    unsigned* genIndices() override;
    //unsigned color;
    int drawOrder = 0;
    Rect mRect;
};

class Geo_Sphere2D : Geo_RenderObj {
    //unsigned color;
    int drawOrder;
    Circle circle;
};

// More Complex types

struct Topl_BaseEntity { // Acts as a node
	Topl_BaseEntity() {
		mId_count++;
		mId = mId_count;
	}
	~Topl_BaseEntity() { mId_count--; }

	// const unsigned const* mId_ref = &mId;
	unsigned getId() const { return mId; }
    Topl_BaseEntity* getParent() const { return mParent; }
    Topl_BaseEntity* getChild(unsigned childNum) const {
        if(childNum > mChildCount) return nullptr;
        else return *(mChild + childNum - 1);
    }

    void updateLocation(Eigen::Vector3f vec); // Follow by more spatial update things
    // ADD CHILD FUNCTION
private:
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id

    Topl_BaseEntity* mParent = nullptr;
    unsigned mChildCount = 0;
    Topl_BaseEntity** mChild = nullptr;

    Eigen::Vector3f relWorldPos; // Positions by which to offset
    Eigen::Vector3f objOrientAngl; // Angles by which to rotate
};

struct Topl_GeoEntity : Topl_BaseEntity {
	Topl_GeoEntity(const Geo_Rect2D* geoObj) { mType.gRect = geoObj; }
    Topl_GeoEntity(const Geo_Sphere2D* geoObj) { mType.gSphere = geoObj; }
    // A unique constructor must exist for every geometry type
    enum GeoBehavior {
        GEO_Fixed = 1,
        GEO_Dynamic = 2
    } mBehavior;
    union GeoType {
        const Geo_Rect2D* gRect;
        const Geo_Sphere2D* gSphere;
        // ADD MORE TYPES OF GEOMETRY
    } mType;
};

class Topl_SceneGraph {
public:
    Topl_SceneGraph(){}
    ~Topl_SceneGraph(){}

    void addGeometry(const char* name, const Topl_GeoEntity* geoEntity);
    // void addTextures(const char* name, const Topl_Texture** textures);
    // void addShaders(const char* name, const Topl_Shader** shaders);
private:
    // unsigned mKeyCount = 0;
    std::map<const char*, unsigned> mNameToId_map; // Associates names to object by IDs
    std::map<unsigned, const Topl_GeoEntity*> mIdToGeo_map;
    // std::map<unsigned, const bool*> mIdToUpdateStat; // DOES OBJECT REQUIRE UPDATING
    // std::map<unsigned, const Topl_Texture**> mIdToTextures_map // WILL MAP TEXTURES TO OBJECTS
    // std::map<unsigned, const Topl_Shader**> mIdToShaders_map // WILL MAP SHADERS TO OBJECTS
}; 