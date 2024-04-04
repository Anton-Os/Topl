#ifndef GEO_ACTOR_H

#include "Geo_Mesh.hpp"

// Actor

#define DEFAULT_ACTOR_ID 0xFFFFFFFF
#define DEFAULT_ACTOR_NAME "actor"

extern class Geo_Actor;
extern struct Topl_EntryShader;

typedef void (*pickerCallback)(Geo_Actor*);
typedef void (*shaderCallback)(Topl_EntryShader*);

class Geo_Actor {
public:
	// Geo_Actor(){ _id = (uint32_t)((0xFF << 24) + ((rand() % 255) << 16) + ((rand() % 255) << 8) + (rand() % 255)); }
	Geo_Actor(){ _id = colorID(); }
	Geo_Actor(const Geo_Mesh* mesh) { 
		// _id = (uint32_t)((0xFF << 24) + ((rand() % 255) << 16) + ((rand() % 255) << 8) + (rand() % 255));
		_id = colorID();
		_mesh = mesh; 
	}
	Geo_Actor(const Geo_Actor& actor){
		_id = actor.getId();
		_name = actor.getName();
		_mesh = actor.getMesh();
		_position = *actor.getPos();
		_rotation = *actor.getRot();
		_scale = *actor.getSize();
	}

	void setName(const std::string& name) { _name = name; }
	void setMesh(const Geo_Mesh* mesh) { _mesh = mesh; }
	void setPos(const Vec3f& moveVec){ _position = moveVec; }
	void updatePos(const Vec3f& moveVec){ _position = _position + moveVec; }
	void setRot(const Vec3f& angleVec){ _rotation = angleVec; }
	void updateRot(const Vec3f& angleVec){ _rotation = _rotation + angleVec; }
	void setSize(const Vec3f& scaleVec){ _scale = scaleVec; }
	void updateSize(const Vec3f& scaleVec){ _scale = _scale + scaleVec; }

	unsigned getId() const { return _id; }
	std::string getName() const { return _name; }
	const Geo_Mesh* getMesh() const { return _mesh; }
	vec3f_cptr_t getPos() const { return &_position; }
	vec3f_cptr_t getRot() const { return &_rotation; }
	vec3f_cptr_t getSize() const { return &_scale; }

	bool isShown = true;
	pickerCallback pickerFunc = nullptr; // invoked on color picking operation
	shaderCallback shaderFunc = nullptr; // invoked during shader render block

	// Add texture object?
	// Add physics object?
private:
	// Data types
	unsigned _id = DEFAULT_ACTOR_ID; // id used for color picking
	std::string _name = DEFAULT_ACTOR_NAME; // user-friendly name
	const Geo_Mesh* _mesh = nullptr; // mesh stored internally

	// Orientation types
	Vec3f _position = Vec3f({ 0.0f, 0.0f, 0.0f });
	Vec3f _rotation = Vec3f({ 0.0f, 0.0f, 0.0f });
	Vec3f _scale = Vec3f({ 1.0f, 1.0f, 1.0f });
};

typedef const Geo_Actor* const actor_cptr;

#define GEO_ACTOR_H
#endif