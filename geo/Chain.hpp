#ifdef RASTERON_H

struct Geo_Chain_Properties {
    Geo_Chain_Properties(){ }
    float distance = 1.0f;
}

class Geo_Chain : public Geo_Construct {
public:
    Geo_Chain(
        const std::string& prefix, 
        Topl_SceneManager* sMan, 
        const Geo_Component* geocLink, 
        const Geo_Chain_Properties* properties,
        unsigned linkCount)
    : Geo_Construct(prefix, sMan, geocLink, linkCount){
        // TODO: Create chain over here

        // fillSceneManager(sMan);
    }
};

#endif