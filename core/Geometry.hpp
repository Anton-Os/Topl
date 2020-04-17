#include <vector>

typedef struct{
    unsigned id;
    float worldSpace[3];
} Topl_BaseEntity;

struct Topl_Object : Topl_BaseEntity{};
struct Topl_Character : Topl_BaseEntity{};
struct Topl_Terrain : Topl_BaseEntity{};
struct Topl_Light : Topl_BaseEntity{};

struct Topl_SceneGraph {
    //Topl_Object* objects;
    //Topl_Character* characters;
    std::vector<Topl_Object> objects;
    std::vector<Topl_Character> characters;
    std::vector<Topl_Light> lights;
    Topl_Terrain terrain;
}; // It is very bare-bones at the moment