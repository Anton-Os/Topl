#include "Geo_Cone.hpp"

// Functions

Geo_Vertex* genCone_vertices(Shape2D shape, Vec3f apex){
	unsigned count = shape.segments + 2;
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

	*(vertexData) = Geo_Vertex({ 0.0f, 0.0f, DEFAULT_Z + 0.00001F }, { 0.5f, 0.5f, 0.0f }, { 0.0F, 0.0F, -1.0F }, { 1.0F, 1.0F, 1.0F }); // origin
    *(vertexData + (count - 1)) = Geo_Vertex(apex, { 0.5f, 0.5f, 1.0f }, { 0.0F, 0.0F, 1.0F }, { 1.0F, 1.0F, 1.0F }); // apex

	unsigned v;
	for (v = 1; v < count - 1; v++) {
		Vec3f pos = Vec3f({ 
			(float)sin(ANGLE_START(shape.segments) + (v * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)cos(ANGLE_START(shape.segments) + (v * ANGLE_OFFSET(shape.segments))) * RADIUS_SIZE(shape.radius),
			(float)DEFAULT_Z
		});

		// Vec3f normal = Vec3f({ 0.0f, 0.0f, -1.0f }); // base facing normal
		*(vertexData + v) = Geo_Vertex(pos);
	}

	return vertexData;
}

Geo_Vertex* genConeP_vertices(vertex_cptr_t points, unsigned c, Vec3f apex) {
	unsigned count = c + 1;
	Geo_Vertex* vertexData = (Geo_Vertex*)malloc(count * sizeof(Geo_Vertex));

	*(vertexData + count - 1) = Geo_Vertex(apex, { 0.5f, 0.5f, 1.0f }, { 0.0F, 0.0F, 1.0F }, { 1.0F, 1.0F, 1.0F }); // apex

	unsigned short v = 0;
	for (unsigned p = 0; p < count; p++) {
		*(vertexData + v) = *(points + p);
		v++;
	}

	return vertexData;
}

unsigned* genCone_indices(Shape2D shape){
	unsigned vCount = shape.segments + 2;
	unsigned iCount = shape.segments * 6;
	unsigned* indexData = (unsigned*)malloc(iCount * sizeof(unsigned));

	unsigned i; // current index
	unsigned v = 1; // tracks current vertex

	// Base Indexing
	for (i = 0; i < (iCount / 2) - 3; i += 3) {
		*(indexData + i) = 0; // origin
		*(indexData + i + 1) = v; // target
		*(indexData + i + 2) = v + 1; // next vertex
		v++;
	}

	// special case for last base triangle
	*(indexData + i) = 0;
	*(indexData + i + 1) = v;
	*(indexData + i + 2) = 1;

		// Apex Indexing
	v = 1; // reset

	for (i = iCount / 2; i < iCount - 3; i += 3) {
		*(indexData + i) = vCount - 1; // apex
		*(indexData + i + 1) = v + 1; // next vertex
		*(indexData + i + 2) = v; // target
		v++;
	}

	// special case for last apex triangle
	*(indexData + i) = vCount - 1;
	*(indexData + i + 1) = v;
	*(indexData + i + 2) = 1;

	return indexData;
}

// Constructors

Geo_Cone::Geo_Cone(Shape2D shape, Vec3f apex) : Geo_Mesh(
	shape.segments + 2, genCone_vertices(shape, apex),
	shape.segments * 6, genCone_indices(shape)
) {
	_shape = shape;
	_apex = apex;
}

Geo_ExtCone::Geo_ExtCone(Shape2D shape, Vec3f apex, unsigned short iters) : Geo_Cone(shape, apex){
	_iters = iters;

    for(unsigned l = 0; l < iters; l++){ // TODO: Push new face vertex?
        unsigned short svCount = getVertexCount(); // start vertex count
    	unsigned short siCount = getIndexCount(); // start index count
		
		for(unsigned p = 0; p < shape.segments; p++){
            Geo_Vertex newVertex = _vertices[p + (l * shape.segments) + 1];
            newVertex.position.data[0] *= 1.0 + (shape.radius * 0.5);
            newVertex.position.data[1] *= 1.0 + (shape.radius * 0.5);
            newVertex.position.data[2] -= apex.data[2];
            _vertices.push_back(newVertex);
        }

        for(unsigned i = 1; i < siCount - 1; i++){ // indexing sides
            // New Vertexing
			_indices.push_back(i + (l * shape.segments));
			_indices.push_back(i + (l * shape.segments) + 1);
			_indices.push_back(i + (l * shape.segments) + svCount - 1);
			_indices.push_back(i + (l * shape.segments) + svCount);
			_indices.push_back(i + (l * shape.segments) + 1);
			_indices.push_back(i + (l * shape.segments) + svCount - 1);
        }

		_indices.push_back((l * shape.segments) + (svCount - 2));
		_indices.push_back((l * shape.segments) + 1);
		_indices.push_back((l * shape.segments) + (svCount + shape.segments - 1));
		_indices.push_back((l * shape.segments) + svCount);
		_indices.push_back((l * shape.segments) + 1);
		_indices.push_back((l * shape.segments) + (svCount + shape.segments - 1));

        // TODO: Index new face?
    }

	// for(unsigned i = 1; i < siCount; i++) _indices[i] = 0; // clear?
}

Geo_Cone::Geo_Cone(vertex_cptr_t points, unsigned short pointCount, Vec3f apex) : Geo_Mesh(
	pointCount + 1, genConeP_vertices(points, pointCount, apex)
){
	_apex = apex;
}

Geo_ExtCone::Geo_ExtCone(vertex_cptr_t points, unsigned short pointCount, Vec3f apex, unsigned short iters) : Geo_Cone(points, pointCount, apex){
	_iters = iters;

    for(unsigned l = 0; l < iters; l++){ // TODO: Push new face vertex?
        unsigned short svCount = getVertexCount(); // start vertex count
    	unsigned short siCount = getIndexCount(); // start index count
		
		for(unsigned p = 0; p < pointCount; p++){
            Geo_Vertex newVertex = _vertices[p + (l * pointCount) + 1];
            newVertex.position.data[0] *= 1.0 + (apex.data[2] * 0.5); // TODO: Compute radius
            newVertex.position.data[1] *= 1.0 + (apex.data[2] * 0.5); // TODO: Compute radius
            newVertex.position.data[2] -= apex.data[2];
            _vertices.push_back(newVertex);
        }

        for(unsigned i = 1; i < siCount; i++){ // indexing sides
            // New Vertexing
			_indices.push_back(i + (l * pointCount));
			_indices.push_back(i + (l * pointCount) + 1);
			_indices.push_back(i + (l * pointCount) + svCount - 1);
			_indices.push_back(i + (l * pointCount) + svCount);
			_indices.push_back(i + (l * pointCount) + 1);
			_indices.push_back(i + (l * pointCount) + svCount - 1);
        }

		_indices.push_back((l * pointCount) + (svCount - 2));
		_indices.push_back((l * pointCount) + 1);
		_indices.push_back((l * pointCount) + (svCount + pointCount - 1));
		_indices.push_back((l * pointCount) + svCount);
		_indices.push_back((l * pointCount) + 1);
		_indices.push_back((l * pointCount) + (svCount + pointCount - 1));

        // TODO: Index new face?
    }
}
