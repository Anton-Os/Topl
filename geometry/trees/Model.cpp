#include "Model.hpp"

Geo_Node Geo_Model::_dummyGeo = Geo_Node(nullptr, nullptr);

namespace _Model {
    std::string genNodeName(unsigned num){ return "node" + std::to_string(num); }
}

void Geo_Model::fill(Topl_Scene* scene){
	Assimp::Importer aiImporter;
	unsigned aiFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;

	const aiScene* aiScene = aiImporter.ReadFile(
		_filePath,
		aiFlags
	);

	_nodes.assign(aiScene->mRootNode->mNumChildren, Geo_Node(aiScene, aiScene->mRootNode));
	for (unsigned c = 0; c < aiScene->mRootNode->mNumChildren; c++)
		_nodes[c] = Geo_Node(aiScene, aiScene->mRootNode->mChildren[c]);

	for (std::vector<Geo_Node>::iterator currentNode = _nodes.begin(); currentNode < _nodes.end(); currentNode++)
		if (currentNode->getMeshCount() == 1) {
			// currentNode->setRenderObj((Geo_RenderObj*)currentNode->getFirstMesh());
			// scene->addGeometry(currentNode->getName(), &(*currentNode));
		}
		else if (currentNode->getMeshCount() > 1)
			puts("Multpile meshes detected!");

}