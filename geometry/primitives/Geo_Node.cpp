#include "Geo_Node.hpp"

unsigned Geo_Mesh::getIndexCount(const aiMesh* mesh){
	unsigned iCount = 0;
	for(unsigned f = 0; f < mesh->mNumFaces; f++){
		const aiFace* face = mesh->mFaces + f;
		iCount += face->mNumIndices;
	}
	return iCount;
}

void Geo_Mesh::genPos(Vec3f* data){
	if(_assimpMeshRef->HasPositions())
		for(unsigned v = 0; v < _verticesCount; v++){
			aiVector3D* vertex = _assimpMeshRef->mVertices + v;
			// *(data + v) = Vec3f(vertex->x, vertex->y, vertex->z);
			*(data + v) = Vec3f({ vertex->x, vertex->y, vertex->z }) * MESH_SCALE;
		}
}

void Geo_Mesh::genNormals(Vec3f* data) {
	if(_assimpMeshRef->HasNormals())
		for(unsigned n = 0; n < _verticesCount; n++){
			aiVector3D* normal = _assimpMeshRef->mNormals + n;
			*(data + n) = Vec3f({ normal->x, normal->y, normal->z });
		}
}

void Geo_Mesh::genTexCoords(Vec2f* data) {
	if(_assimpMeshRef->HasTextureCoords(0))
		for(unsigned t = 0; t < _verticesCount; t++){
			aiVector3D* texcoord = *(_assimpMeshRef->mTextureCoords) + t;
			*(data + t) = Vec2f({ texcoord->x, texcoord->y });
		}
}

void Geo_Mesh::genIndices(unsigned* data){
	unsigned i = 0;
	if(_assimpMeshRef->HasFaces())
	for(unsigned f = 0; f < _assimpMeshRef->mNumFaces; f++){
		const aiFace* face = _assimpMeshRef->mFaces + f;
		for(unsigned n = 0; n < face->mNumIndices; n++){
			*(data + i) = *(face->mIndices + n); // index into the current face
			i++;
		}
	}
}

Geo_Node::Geo_Node(const aiScene* scene, const aiNode* node) : Geo_Actor() {
	if (scene != nullptr && node != nullptr) {
		setName(std::string(node->mName.C_Str()));
		_scene = scene;
		_node = node;

		_meshCount = node->mNumMeshes;
		if (_meshCount == 1) {
			Geo_Mesh mesh = Geo_Mesh(*(_scene->mMeshes + *(_node->mMeshes)));
			_mesh->cloneMesh(&mesh);
			setRenderObj((Geo_RenderObj*)_mesh);
		}
		else if (_meshCount > 1) {
			puts("Multiple meshes detected!");
			// TODO: implement code to fuse multiple meshes together!

			Geo_Mesh mesh = Geo_Mesh(*(_scene->mMeshes + *(_node->mMeshes)));
			_mesh->cloneMesh(&mesh);
			setRenderObj((Geo_RenderObj*)_mesh);
		}
		else puts("No meshes detected!");
	}
}