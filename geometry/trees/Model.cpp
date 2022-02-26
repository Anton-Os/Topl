#include "Model.hpp"

Geo_Node Geo_Model::_dummyGeo = Geo_Node(nullptr, nullptr);

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

void Geo_Model::init(Topl_Scene* scene){
	Assimp::Importer aiImporter;
	unsigned aiFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;

	const aiScene* aiScene = aiImporter.ReadFile(
		_filePath,
		aiFlags
	);

	_nodeCount = aiScene->mRootNode->mNumChildren;
	_nodes = (Geo_Node**)malloc(_nodeCount * sizeof(Geo_Node*));
	for (unsigned n = 0; n < _nodeCount; n++) 
		*(_nodes + n) = new Geo_Node(aiScene, aiScene->mRootNode->mChildren[n]);

	for (unsigned n = 0; n < _nodeCount; n++) {
		Geo_Node* currentNode = *(_nodes + n);
		if (currentNode->getMeshCount() > 0) {
			_geoNodeList.push_back(currentNode);
			scene->addGeometry(getPrefix() + currentNode->getName(), currentNode);
		}
	}
}