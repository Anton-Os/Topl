#include "Geo_Mesh.hpp"

class Geo_Volume : public Geo_Mesh {
public:
	Geo_Volume(Shape2D shape)
	: Geo_Mesh((shape.segments + 1) * 2, shape.segments * 12) {
		_shape = shape;
		_depth = shape.radius;
		genVertices(); genIndices();
	}

	Geo_Volume(Shape2D shape, float depth) 
	: Geo_Mesh((shape.segments + 1) * 2, shape.segments * 12) {
		_shape = shape;
		_depth = depth;
		genVertices(); genIndices();
	}
private:
	void genVertices() override;
	void genIndices() override;

	Shape2D _shape;
	float _depth = DEFAULT_Z;
};

struct Geo_VolumeTriangle : public Geo_Volume {
	Geo_VolumeTriangle() : Geo_Volume({ 1.0, 3 }) {}
	Geo_VolumeTriangle(float radius) : Geo_Volume({ radius, 3 }) {}
	Geo_VolumeTriangle(float radius, float depth) : Geo_Volume({ radius, 3 }, depth) {}
};

struct Geo_VolumeSquare : public Geo_Volume { // Box
	Geo_VolumeSquare() : Geo_Volume({ 1.0, 4 }) {}
	Geo_VolumeSquare(float radius) : Geo_Volume({ radius, 4 }) {}
	Geo_VolumeSquare(float radius, float depth) : Geo_Volume({ radius, 4 }, depth) {}
};

struct Geo_VolumeHex : public Geo_Volume {
	Geo_VolumeHex() : Geo_Volume({ 1.0, 6 }) {}
	Geo_VolumeHex(float radius) : Geo_Volume({ radius, 6 }) {}
	Geo_VolumeHex(float radius, float depth) : Geo_Volume({ radius, 6 }, depth) {}
};

struct Geo_VolumeCircle : public Geo_Volume { // Cylinder
	Geo_VolumeCircle() : Geo_Volume({ 1.0, CIRCLE_SEGMENTS }) {}
	Geo_VolumeCircle(float radius) : Geo_Volume({ radius, CIRCLE_SEGMENTS }) {}
	Geo_VolumeCircle(float radius, float depth) : Geo_Volume({ radius, CIRCLE_SEGMENTS }, depth) {}
};