#include "Geo_Model.hpp"

namespace _Model {
    std::string genNodeName(unsigned num){ return "node" + std::to_string(num); }
}

Geo_Model::~Geo_Model(){
	if(_nodes != nullptr){
		for(unsigned n = 0; n < _nodeCount; n++) 
			delete(*(_nodes + n));
		free(_nodes);
		_nodes = nullptr;
	}
}

void Geo_Model::configure(Topl_Scene* scene){
	Assimp::Importer aiImporter;
	unsigned aiFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;

	const aiScene* aiScene = aiImporter.ReadFile(
		_filePath,
		aiFlags
	);

	// All Nodes
	_nodeCount = aiScene->mRootNode->mNumChildren;
	_nodes = (Geo_Node**)malloc(_nodeCount * sizeof(Geo_Node*));
	for (unsigned n = 0; n < _nodeCount; n++) 
		*(_nodes + n) = new Geo_Node(aiScene, aiScene->mRootNode->mChildren[n]);

	// Geometry Only Nodes
	for (unsigned n = 0; n < _nodeCount; n++) {
		Geo_Node* currentNode = *(_nodes + n);
		if (currentNode->getMesh()->getMeshCount() > 0) {
			_geoNodes.push_back(currentNode);
			scene->addGeometry(getPrefix() + currentNode->getName(), currentNode);
		}
	}
}