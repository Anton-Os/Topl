#include "Geo_Node.hpp"

// Mesh

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

void Geo_Mesh::genVertices() {
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

			_vertices[v] = Geo_Vertex(pos, texcoord);
		}
}

void Geo_Mesh::genIndices() {
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

// Node

Geo_Node::Geo_Node(const aiScene* scene, const aiNode* node) : Geo_Actor() {
	if (scene != nullptr && node != nullptr) {
		setName(std::string(node->mName.C_Str()));
		_scene = scene;
		_node = node;
		
		if (node->mNumMeshes == 1) {
			const aiMesh* refMesh = *(_scene->mMeshes + *(_node->mMeshes));
			_mesh = new Geo_Mesh(refMesh);
		} else if(node->mNumMeshes > 1){
			std::vector<const aiMesh*> refMeshes = std::vector<const aiMesh*>(node->mNumMeshes);
			for(unsigned m = 0; m < node->mNumMeshes; m++) // iterates through all meshes within current node
				refMeshes[m] = *(_scene->mMeshes + *(_node->mMeshes + m)); 
			_mesh = new Geo_Mesh(refMeshes);
		}
		else puts("No meshes detected!");

		if(_mesh != nullptr) setMesh((Geo_Mesh*)_mesh);
	}
}