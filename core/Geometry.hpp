#include <vector>
#include <map>

/* typedef struct{
    unsigned id;
    float worldSpace[3];
} Topl_BaseEntity; */

struct Topl_BaseEntity {
	Topl_BaseEntity() {
		mId_count++;
		mId = mId_count;
	}
	~Topl_BaseEntity() { mId_count--; }

	// const unsigned const* mId_ref = &mId;
	unsigned get_Id() const { return mId; }
    float mWorldSpace[3]; // Replace with eigen datatype
private:
	static unsigned mId_count; // Grows/shrinks when objects are created/deleted
	unsigned mId; // Each object has a unique id
};



// unsigned Topl_BaseEntity::mId_count = 0; // INCLUDE THIS IN CPP FILE!

// Topl_BaseEntity::mId_count = 0; // Add this somewhere!!!

struct Topl_RectSprite{
    // Topl_RectSprite(const Rasteron_Sprite(const*) s){}
private:
    // const Rasteron_Sprite(const*) sprite; // BUILD RASTERON
    float width;
    float height;
};

struct Topl_Object : Topl_BaseEntity{};
struct Topl_Character : Topl_BaseEntity{};
struct Topl_Terrain : Topl_BaseEntity{};
struct Topl_Light : Topl_BaseEntity{};


struct Topl_GeoEntity : Topl_BaseEntity {
    enum GeoBehavior {
        GEO_Fixed = 1,
        GEO_Dynamic = 2
    } behavior;
    union GeoType {
        Topl_RectSprite rectSprite;
        // ADD MORE TYPES OF GEOMETRY
    } type;
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
    void constructScene();
private:
    // unsigned mKeyCount = 0;
    std::map<const char*, unsigned> mNameToId_map; // Associates names to object by IDs
    std::map<unsigned, const Topl_GeoEntity*> mIdToObj_map;

    //Topl_Object* objects;
    //Topl_Character* characters;
    std::vector<Topl_Object> objects;
    std::vector<Topl_Character> characters;
    std::vector<Topl_Light> lights;
    Topl_Terrain terrain;
}; // It is very bare-bones at the moment