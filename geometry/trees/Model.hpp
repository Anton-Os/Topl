#include "Geo_Tree.hpp"
#include "primitives/Geo_Node.hpp"

class Geo_Model : public Geo_Tree {
public:
    Geo_Model(
		const std::string& prefix,
        std::string filePath,
        Topl_Scene* scene
    ) : Geo_Tree(prefix, scene, &_dummyGeo, 1){
        _filePath = filePath;

        fill(scene);
    }
private:
    void fill(Topl_Scene* scene) override;

    static Geo_Node _dummyGeo;
    std::string _filePath; // nodes are read from file
    std::vector<Geo_Node> _nodes;
};