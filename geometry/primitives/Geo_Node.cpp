#include "Geo_Node.hpp"

unsigned getIndexCount(const aiMesh* mesh){
	unsigned iCount = 0;
	for(unsigned f = 0; f < mesh->mNumFaces; f++){
		const aiFace* face = mesh->mFaces + f;
		iCount += face->mNumIndices;
	}
	return iCount;
}

void Geo_Mesh::genPos(Vec3f* data){
	// for(std::vector<const aiMesh*>::const_iterator mesh = _assimpMeshRefs.cbegin(); mesh < _assimpMeshRefs.cend(); mesh++)
	if(_assimpMeshRef->HasPositions())
		for(unsigned v = 0; v < _vertexCount; v++){
			aiVector3D* vertex = _assimpMeshRef->mVertices + v;
			// *(data + v) = Vec3f(vertex->x, vertex->y, vertex->z);
			*(data + v) = Vec3f({ vertex->x, vertex->y, vertex->z }) * MESH_SCALE;
		}
}

void Geo_Mesh::genNormals(Vec3f* data) {
	// for(std::vector<const aiMesh*>::const_iterator mesh = _assimpMeshRefs.cbegin(); mesh < _assimpMeshRefs.cend(); mesh++)
	if(_assimpMeshRef->HasNormals())
		for(unsigned n = 0; n < _vertexCount; n++){
			aiVector3D* normal = _assimpMeshRef->mNormals + n;
			*(data + n) = Vec3f({ normal->x, normal->y, normal->z });
		}
}

void Geo_Mesh::genTexCoords(Vec2f* data) {
	// for(std::vector<const aiMesh*>::const_iterator mesh = _assimpMeshRefs.cbegin(); mesh < _assimpMeshRefs.cend(); mesh++)
	if(_assimpMeshRef->HasTextureCoords(0))
		for(unsigned t = 0; t < _vertexCount; t++){
			aiVector3D* texcoord = *(_assimpMeshRef->mTextureCoords) + t;
			*(data + t) = Vec2f({ texcoord->x, texcoord->y });
		}
}

void Geo_Mesh::genIndices(unsigned* data){
	unsigned i = 0;

	// for(std::vector<const aiMesh*>::const_iterator mesh = _assimpMeshRefs.cbegin(); mesh < _assimpMeshRefs.cend(); mesh++)
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
			const aiMesh* refMesh = *(_scene->mMeshes + *(_node->mMeshes));
			
			_mesh = new Geo_Mesh(refMesh);
			setRenderObj((Geo_RenderObj*)_mesh);
		} else if(_meshCount > 1){
			std::vector<const aiMesh*> refMeshes = std::vector<const aiMesh*>(_meshCount);
			for(unsigned m = 0; m < _meshCount; m++) // iterates through all meshes within current node
				refMeshes[m] = *(_scene->mMeshes + *(_node->mMeshes + m)); 
			
			_mesh = new Geo_Mesh(refMeshes);
			setRenderObj((Geo_RenderObj*)_mesh);
		}
		else puts("No meshes detected!");
	}
}