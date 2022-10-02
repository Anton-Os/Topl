#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

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

	void move(Vec3f vec) { // replace Geo_Tree move()
		for (std::vector<Geo_Node*>::iterator n = _geoNodes.begin(); n != _geoNodes.end(); n++)
			(*n)->updatePos({ vec });
	}
    void configure(Topl_Scene* scene) override;
private:
    std::string _filePath; // nodes are read from file

    std::vector<Geo_Node*> _geoNodes; // geometry nodes only
    unsigned _nodeCount = 0; // all nodes count
    Geo_Node** _nodes = nullptr; // all nodes data

    // TODO: include animation data
};