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
	for(unsigned v = 0; v < _verticesCount; v++){
		aiVector3D* vertex = _mesh->mVertices + v;
		*(data + v) = Eigen::Vector3f(vertex->x, vertex->y, vertex->z);
	}
}

void Geo_Mesh::genNormals(Eigen::Vector3f* data) {
	for(unsigned n = 0; n < _verticesCount; n++){
		aiVector3D* normal = _mesh->mNormals + n;
		*(data + n) = Eigen::Vector3f(normal->x, normal->y, normal->z);
	}
}

void Geo_Mesh::genTexCoords(Eigen::Vector2f* data) {
	for(unsigned t = 0; t < _verticesCount; t++){
		// aiVector3D* texcoord = _mesh->mTextureCoords + t;
		// *(data + t) = Eigen::Vector2f(texcoord->x, texcoord->y);
		*(data + t) = Eigen::Vector2f(0.0f, 0.0f); // for testing
	}
}

void Geo_Mesh::genIndices(unsigned* data){
	unsigned i = 0;
	for(unsigned f = 0; f < _mesh->mNumFaces; f++){
		const aiFace* face = _mesh->mFaces + f;
		for(unsigned n = 0; n < face->mNumIndices; n++){
			*(data + i) = *(face->mIndices + n); // index into the current face
			i++;
		}
	}
}

// Internal Initialization Code

void Geo_Node::init(const aiScene* scene, const aiNode* node){
	setName(std::string(node->mName.C_Str()));
	_scene = scene;
	_node = node;

	/* _meshCount = node->mNumMeshes;
	if(_meshCount > 0){
		_meshes = (Geo_Mesh*)malloc(_meshCount * sizeof(Geo_Mesh));
		for(unsigned m = 0; m < _meshCount; m++) // retrives mesh from correct index
			*(_meshes + m) = Geo_Mesh(*(_scene->mMeshes + (*(_node->mMeshes + m))));
	} */

	_meshCount = node->mNumMeshes;
	if (_meshCount == 1) {
		_mesh = Geo_Mesh(*(_scene->mMeshes + *(_node->mMeshes)));
		setRenderObj((Geo_RenderObj*)&_mesh);
	}
	else if(_meshCount > 1) puts("Multiple meshes detected!");
	else puts("No meshes detected!");
}