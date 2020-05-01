#include <vector>
#include <map>

// Simple Primitive Types

struct Topl_Circle {
    float radius;
    float segments;
};

struct Topl_Rect {
    float width;
    float height;
};

// Objects that are candidates for Topl_GeoEntity::GeoType
// Topl_BallSprite should be added

struct Topl_RectSprite{
    // Topl_RectSprite(const Rasteron_Sprite(const*) s){}
private:
    // const Rasteron_Sprite(const*) sprite; // BUILD RASTERON
    unsigned color; // For now for testing
    int drawOrder; // To create a draw order
    Topl_Rect rect;
};

// More Complex types

/* struct Topl_Object : Topl_BaseEntity{};
struct Topl_Character : Topl_BaseEntity{};
struct Topl_Terrain : Topl_BaseEntity{};
struct Topl_Light : Topl_BaseEntity{}; */

struct Topl_BaseEntity { // Acts as a node
	Topl_BaseEntity() {
		mId_count++;
		mId = mId_count;
	}
	~Topl_BaseEntity() { mId_count--; }

	// const unsigned const* mId_ref = &mId;
	unsigned get_Id() const { return mId; }

    void updateLocation(float loc[3]); // Follow by more spatial update things
    // ADD CHILD FUNCTION
private:
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id

    Topl_BaseEntity* parent = nullptr;
    unsigned childCount = 0;
    Topl_BaseEntity** child = nullptr;

    float mRelLocation[3]; // Replace with eigen datatype
};

struct Topl_GeoEntity : Topl_BaseEntity {
	Topl_GeoEntity(const Topl_RectSprite* geoObj) { mType.rectSprite = geoObj; }
    // A unique constructor must exist for every geometry type
    enum GeoBehavior {
        GEO_Fixed = 1,
        GEO_Dynamic = 2
    } mBehavior;
    union GeoType {
        const Topl_RectSprite* rectSprite;
        // ADD MORE TYPES OF GEOMETRY
    } mType;
};

/* struct Topl_Plane {
    float xLen;
    unsigned short xDiv;
    float yLen;
    unsigned short yDiv;
    float zLen;
    unsigned short zDiv;
    
    float hSlope; // Horizontal slope
    float vSlope; // Vertical slope
}; // TOO EARLY TO IMPLEMENT */

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
}; // It is very bare-bones at the moment