#include "Geo_Surface.hpp"

// Functions

Geo_Vertex* genSurface_vertices(Shape2D shape, float z){
	unsigned count = shape.segments + 1;
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

	*(vertexData) = Geo_Vertex({ 0.0f, 0.0f, z }, { 0.5f, 0.5f, 0.0f }, { 0.0F, 0.0F, -1.0F }, { 1.0F, 1.0F, 1.0F }); // origin

	unsigned v;
	for (v = 1; v < count; v++) {
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(shape.segments) + ((v - 1) * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)cos(ANGLE_START(shape.segments) + ((v - 1) * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)z
		});

		*(vertexData + v) = Geo_Vertex(pos);
    }

	return vertexData;
}

Geo_Vertex* genSurface_vert(vertex_cptr_t points, unsigned c, float z) {
	unsigned count = c;
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

	unsigned short v = 0;
	for (unsigned p = 0; p < count; p++) {
		*(vertexData + v) = *(points + p);
		(*(vertexData + v)).position[2] = z;
		v++;
	}

	return vertexData;
}

unsigned* genSurface_indices(Shape2D shape){
	unsigned iCount = shape.segments * 3;
	unsigned* indexData = (unsigned*)malloc(iCount * sizeof(unsigned));

	unsigned v = 1; // tracks current vertex
	for (unsigned i = 0; i < iCount - 3; i += 3) {
		*(indexData + i) = 0; // origin
		*(indexData + i + 1) = v; // target
		*(indexData + i + 2) = v + 1; // next vertex
		v++;
	}

	// special case for last triangle
	*(indexData + iCount - 3) = 0;
	*(indexData + iCount - 2) = v;
	*(indexData + iCount - 1) = 1;

	return indexData;
}

// Constructors

Geo_Surface::Geo_Surface(Shape2D shape, float z) : Geo_Mesh(
	shape.segments + 1, genSurface_vertices(shape, z),
	shape.segments * 3, genSurface_indices(shape)
){
	_shape = shape;
	_depth = z;
}

Geo_Ext2D::Geo_Ext2D(Shape2D shape, float z, unsigned short iters) : Geo_Surface(shape, z){
	_iters = iters;

    Geo_Mesh* targetMesh = new Geo_Mesh(*((Geo_Mesh*)this));
	for(unsigned i = 0; i < _iters; i++){
		unsigned short svCount = getVertexCount(); // start vertex count
    	unsigned short siCount = getIndexCount(); // start index count

		for(unsigned v = 0; v < svCount; v++){
			Geo_Mesh* newMesh = new Geo_Mesh(*targetMesh);
			newMesh->shift(_vertices[v].position);
			newMesh->scale({ 1.0F / (i + 2), 1.0F / (i + 2), 1.0F / (i + 2) });
			extend(*newMesh);
			delete newMesh;
		}

		for(unsigned i = 0; i < siCount; i++) _indices[i] = 0;
	}

	delete targetMesh;
}

Geo_Surface::Geo_Surface(vertex_cptr_t points, unsigned short pointCount, float z) : Geo_Mesh(
	pointCount, genSurface_vert(points, pointCount, z)
){
	_depth = z;
}

Geo_Ext2D::Geo_Ext2D(vertex_cptr_t points, unsigned short pointCount, unsigned short iters) : Geo_Surface(points, pointCount, DEFAULT_Z){
	_iters = iters;

    Geo_Mesh* targetMesh = new Geo_Mesh(*((Geo_Mesh*)this));
	for(unsigned i = 0; i < _iters; i++){
		unsigned short svCount = getVertexCount(); // start vertex count
    	unsigned short siCount = getIndexCount(); // start index count

		for(unsigned v = 0; v < svCount; v++){
			Geo_Mesh* newMesh = new Geo_Mesh(*targetMesh);
			newMesh->shift(_vertices[v].position);
			newMesh->scale({ 1.0F / (i + 2), 1.0F / (i + 2), 1.0F / (i + 2) });
			extend(*newMesh);
			delete newMesh;
		}

		for(unsigned i = 0; i < siCount; i++) _indices[i] = 0;
	}

	delete targetMesh;
}
