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

struct Geo_RenderObj {
    unsigned vCount;
    unsigned iCount;

    // Eigen::Vector3d vec; // Work plz
    virtual Eigen::Vector3d* genVertices() = 0;
    virtual unsigned* genIndices() = 0;
};

// Override the virtual functions above
class Geo_Rect2D : Geo_RenderObj {
    //unsigned color;
    int drawOrder;
    Rect rect;
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

    void updateLocation(float loc[3]); // Follow by more spatial update things
    // ADD CHILD FUNCTION
private:
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id

    Topl_BaseEntity* mParent = nullptr;
    unsigned mChildCount = 0;
    Topl_BaseEntity** mChild = nullptr;

    Eigen::Vector3d relWorldPos; // Positions by which to offset
    Eigen::Vector3d objOrientAngl; // Angles by which to rotate
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