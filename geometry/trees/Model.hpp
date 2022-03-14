#include "Geo_Tree.hpp"
#include "primitives/Geo_Node.hpp"

class Geo_Model : public Geo_Tree {
public:
    Geo_Model(
		const std::string& prefix,
        const std::string& filePath,
        Topl_Scene* scene
    ) : Geo_Tree(prefix, &_dummyGeo, 1){
        _filePath = filePath;

        init(scene);
    }
    ~Geo_Model(); 

    void init(Topl_Scene* scene) override;
private:
    static Geo_Node _dummyGeo;
    std::string _filePath; // nodes are read from file

    unsigned _nodeCount = 0;
    Geo_Node** _nodes = nullptr; // dynamically allocated and managed
    std::vector<Geo_Node*> _geoNodeList; // contains only nodes with valid meshes
};