#include "Geo_Tree.hpp"
#include "primitives/Geo_Node.hpp"

class Geo_Model : public Geo_Tree {
public:
    Geo_Model( // Prebake Constructor
		const std::string& prefix,
        const std::string& filePath
    ) : Geo_Tree(prefix){
        _filePath = filePath;
    }

    Geo_Model( // Config Constructor
		const std::string& prefix,
        const std::string& filePath,
        Topl_Scene* scene
    ) : Geo_Tree(prefix){
        _filePath = filePath;
        configure(scene);
    }

    ~Geo_Model();

    void configure(Topl_Scene* scene) override;
private:
    static Geo_Node _dummyGeo;
    std::string _filePath; // nodes are read from file

    unsigned _nodeCount = 0;
    Geo_Node** _nodes = nullptr; // dynamically allocated and managed
    std::vector<Geo_Node*> _geoNodeList; // contains only nodes with valid meshes
};