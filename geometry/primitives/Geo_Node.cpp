#include "Geo_Node.hpp"

unsigned Geo_Mesh::getIndexCount(const aiMesh* mesh){
	unsigned iCount = 0;
	for(unsigned f = 0; f < mesh->mNumFaces; f++){
		const aiFace* face = mesh->mFaces + f;
		iCount += face->mNumIndices;
	}
	return iCount;
}

void Geo_Mesh::genVertices(Eigen::Vector3f* data){
	// TODO: Implement body here
	return;
}

void Geo_Mesh::genNormals(Eigen::Vector3f* data) {
	// TODO: Implement body here
	return;
}

void Geo_Mesh::genTexCoords(Eigen::Vector2f* data) {
	// TODO: Implement body here
	return;
}

void Geo_Mesh::genIndices(unsigned* data){
	// TODO: Implement body here
	return;
}

// Internal Initialization Code

void Geo_Node::init(const aiScene* scene, const aiNode* node){
	setName(std::string(node->mName.C_Str()));
	_scene = scene;
	_node = node;

	_meshCount = node->mNumMeshes;
	if(_meshCount > 0){
		_meshes = (Geo_Mesh*)malloc(_meshCount * sizeof(Geo_Mesh));
		for(unsigned m = 0; m < _meshCount; m++) // retrives mesh from correct index
			*(_meshes + m) = Geo_Mesh(*(_scene->mMeshes + (*(_node->mMeshes + m))));
	}
}