// #ifdef TOPL_ENABLE_MODELS

#include "Geo_Node.hpp"

// Node Mesh

unsigned getMeshAttribCount(const aiMesh* mesh, MESH_Attribute attrib){
	unsigned count = 0;

	switch(attrib){
		case MESH_Vertex: return mesh->mNumVertices; // gets vertex count for single mesh
		case MESH_Index: // gets index count for single mesh
			for(unsigned f = 0; f < mesh->mNumFaces; f++){
				const aiFace* face = mesh->mFaces + f;
				count += face->mNumIndices;
			}
			return count;
		default: return 0; // Attribute not available
	}
}

unsigned getMeshesAttribCount(const std::vector<const aiMesh*>& meshes, MESH_Attribute attrib){
	unsigned count = 0;
	
	switch(attrib){
		case MESH_Vertex: // gets vertex count for multiple meshes
			for(std::vector<const aiMesh*>::const_iterator mesh = meshes.cbegin(); mesh < meshes.cend(); mesh++)
				count += getMeshAttribCount(*mesh, MESH_Vertex);
			return count;
		case MESH_Index: // gets index count for multiple meshes
			for(std::vector<const aiMesh*>::const_iterator mesh = meshes.cbegin(); mesh < meshes.cend(); mesh++)
				count += getMeshAttribCount(*mesh, MESH_Index);
			return count;
		default: return 0; // Attribute not available
	}
}

void Geo_NodeMesh::init(){
	for (std::vector<const aiMesh*>::const_iterator mesh = _assimpMeshes.cbegin(); mesh < _assimpMeshes.cend(); mesh++)
		for (unsigned v = 0; v < _vertices.size(); v++) {
			Vec3f pos, normal;
			Vec3f texcoord;
			if ((*mesh)->HasPositions()) {
				aiVector3D* attrib = (*mesh)->mVertices + v;
				// pos = Vec3f({ attrib->x, attrib->y, attrib->z });
				pos = Vec3f({ attrib->x, attrib->y, attrib->z }) * MESH_SCALE; // scale down for testing
			}
			if ((*mesh)->HasNormals()) {
				aiVector3D* attrib = (*mesh)->mNormals + v;
				normal = Vec3f({ attrib->x, attrib->y, attrib->z });
			}
			if ((*mesh)->HasTextureCoords(0)) {
				aiVector3D* attrib = *((*mesh)->mTextureCoords) + v;
				texcoord = Vec3f({ attrib->x, attrib->y, attrib->z });
			}

			_vertices[v] = Geo_Vertex(pos, texcoord, normal, Vec3f({ 1.0F, 1.0F, 1.0F }));
		}

	unsigned i = 0;

	for (std::vector<const aiMesh*>::const_iterator mesh = _assimpMeshes.cbegin(); mesh < _assimpMeshes.cend(); mesh++)
		if ((*mesh)->HasFaces())
			for (unsigned f = 0; f < (*mesh)->mNumFaces; f++) {
				const aiFace* face = (*mesh)->mFaces + f;
				for (unsigned n = 0; n < face->mNumIndices; n++) {
					_indices[i] = *(face->mIndices + n); // index in current face
					i++;
				}
			}
}

// Node Actor

void Geo_NodeActor::init(const aiScene* scene, const aiNode* node) {
	_sceneRef = scene;
	_nodeRef = node;

	setName(std::string(_nodeRef->mName.C_Str()));
	setPos({ _nodeRef->mTransformation.a4 * MESH_SCALE, _nodeRef->mTransformation.b4 * MESH_SCALE - 1.0F, _nodeRef->mTransformation.c4 * MESH_SCALE });
	setRot({ 0.0f, 0.0f, 0.0f}); // TODO: Determine values from transformation matrix
	setSize({ _nodeRef->mTransformation.a1, _nodeRef->mTransformation.b2, _nodeRef->mTransformation.c3 });
	
	if (_nodeRef->mNumMeshes == SINGLE_NODE_COUNT) {
		const aiMesh* refMesh = *(_sceneRef->mMeshes + *(_nodeRef->mMeshes));
		_mesh = new Geo_NodeMesh(refMesh);
	} else if(_nodeRef->mNumMeshes > SINGLE_NODE_COUNT){
		std::vector<const aiMesh*> refMeshes = std::vector<const aiMesh*>(_nodeRef->mNumMeshes);
		for(unsigned m = 0; m < _nodeRef->mNumMeshes; m++) // iterates through all meshes within current node
			refMeshes[m] = *(_sceneRef->mMeshes + *(_nodeRef->mMeshes + m)); 
		_mesh = new Geo_NodeMesh(refMeshes);
	}

	if(_mesh != nullptr) setMesh((Geo_Mesh*)_mesh);
}

// #endif