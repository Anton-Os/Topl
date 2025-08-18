#include "Geo_Volume.hpp"

// Functions

Geo_Vertex* genVolume_vertices(Shape2D shape, float d){
	unsigned count = (shape.segments + 1) * 2;
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));
	
	*(vertexData) = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + (d / 2) }, { 0.5, 0.5, 0.0f }, { 0.0F, 0.0F, -1.0F }, { 1.0F, 1.0F, 1.0F });

	unsigned v = 1;
	for (v = 1; v < count / 2; v++) { // front face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(shape.segments) + (v * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)cos(ANGLE_START(shape.segments) + (v * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)DEFAULT_Z + (d / 2)
		});

		*(vertexData + v) = Geo_Vertex(pos);
	}

    *(vertexData + (count / 2)) = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z - (d / 2) }, { 0.5, 0.5, 1.0f }, { 0.0F, 0.0F, 1.0F }, { 1.0F, 1.0F, 1.0F });
	for (v = 1 + (count / 2); v < count; v++) { // back face vertices
		Vec3f pos = Vec3f({
			(float)sin(ANGLE_START(shape.segments) + ((v - (count / 2)) * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)cos(ANGLE_START(shape.segments) + ((v - (count / 2)) * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)DEFAULT_Z - (d / 2)
		});

		*(vertexData + v) = Geo_Vertex(pos);
	}

	return vertexData;
}

unsigned* genVolume_indices(Shape2D shape){
	unsigned vCount = (shape.segments + 1) * 2;
	unsigned iCount = shape.segments * 12;
	unsigned* indexData = (unsigned*)malloc(iCount * sizeof(unsigned));

	unsigned i; // current index
	unsigned v = 1; // tracks current vertex

	// Front Face Indexing
	for (i = 0; i < (iCount / 4) - 3; i += 3) {
		*(indexData + i) = 0; // origin
		*(indexData + i + 1) = v; // target
		*(indexData + i + 2) = v + 1; // next vertex
		v++;
	}

	*(indexData + i) = 0;
	*(indexData + i + 1) = v;
	*(indexData + i + 2) = 1;

	// Back Face Indexing
	v = (vCount / 2) + 1;
	for (i = iCount / 4; i < (iCount / 2) - 3; i += 3) {
		*(indexData + i) = vCount / 2; // origin
		*(indexData + i + 1) = v; // target
		*(indexData + i + 2) = v + 1; // next vertex
		v++;
	}

	*(indexData + i) = vCount / 2;
	*(indexData + i + 1) = v;
	*(indexData + i + 2) = (vCount / 2) + 1;

	// Side Face Indexing
	v = 1;
	for (i = iCount / 2; i < iCount - 6; i += 6) {
		*(indexData + i) = v;
		*(indexData + i + 1) = v + 1;
		*(indexData + i + 2) = v + (vCount / 2) + 1;
		*(indexData + i + 3) = v + (vCount / 2);
		*(indexData + i + 4) = v;
		*(indexData + i + 5) = v + (vCount / 2) + 1;
		v++;
	}

	*(indexData + i) = 1;
	*(indexData + i + 1) = (vCount / 2) + 1;
	*(indexData + i + 2) = (vCount / 2) - 1;
	*(indexData + i + 3) = vCount - 1;
	*(indexData + i + 4) = (vCount / 2) - 1;
	*(indexData + i + 5) = (vCount / 2) + 1;

	return indexData;
}

Geo_Vertex* genVolume_vert(vertex_cptr_t points, unsigned c, float d) {
	unsigned count = c * 2;
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

	unsigned short v = 0;
	for (unsigned p = 0; p < count; p++) {
		*(vertexData + v) = *(points + p);
		(*(vertexData + v)).position.data[2] += d / 2;
		*(vertexData + count - 1 - v) = *(points + p);
		(*(vertexData + count  - 1 - v)).position.data[2] -= d / 2;
		v++;
	}

	return vertexData;
}

// Constructors

Geo_Volume::Geo_Volume(Shape2D shape, float depth) : Geo_Mesh(
	(shape.segments + 1) * 2, genVolume_vertices(shape, depth), 
	shape.segments * 12, genVolume_indices(shape)
) {
	_shape = shape;
	_depth = depth;
}

Geo_Ext3D::Geo_Ext3D(Shape2D shape, float depth, unsigned short iters) : Geo_Volume(shape, depth){
	_iters = iters;

	Geo_Mesh* targetMesh = new Geo_Mesh(*((Geo_Mesh*)this));
	for(unsigned i = 0; i < _iters; i++){
		Geo_Mesh* newMesh = new Geo_Mesh(*targetMesh);
		newMesh->shift({ 0.0F, 0.0F, (i + 1) * depth });
		extend(*newMesh);
		delete newMesh;
	}

	delete targetMesh;
}

Geo_Volume::Geo_Volume(vertex_cptr_t points, unsigned short pointCount, float depth) : Geo_Mesh(
	pointCount * 2, genVolume_vert(points, pointCount, _depth)
){
	_depth = depth;
}

Geo_Ext3D::Geo_Ext3D(vertex_cptr_t points, unsigned short pointCount, float depth, unsigned short iters) : Geo_Volume(points, pointCount, depth){
	_iters = iters;

	Geo_Mesh* targetMesh = new Geo_Mesh(*((Geo_Mesh*)this));
	for(unsigned i = 0; i < _iters; i++){
		Geo_Mesh* newMesh = new Geo_Mesh(*targetMesh);
		newMesh->shift({ 0.0F, 0.0F, (i + 1) * depth });
		extend(*newMesh);
		delete newMesh;
	}

	delete targetMesh;
}
