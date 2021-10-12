#include "primitives/Geo_Node.hpp"

class Geo_Model : public Geo_Tree {
public:
    Geo_Model(
        const char* filePath,
        const std::string& name,
        Topl_Scene* scene,
        std::vector<const Geo_Node*> nodes
    ) : Geo_Tree(prefix, scene, nullptr, nodes.size()){ // includes 
        _filePath = filePath;

        fill(scene);
    }
private:
    void fill(Topl_Scene* scene) override;

    const char* _filePath; // nodes are read from file
};