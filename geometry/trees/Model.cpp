#include "Model.hpp"

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

	if (aiScene != nullptr) {
		puts("Scene importation failed");
		return;
	}

	// Perform processing here
}