#ifndef GEO_ACTOR_H

#include "Controls.hpp"

#include "Geo_Mesh.hpp"

// Actor

#define DEFAULT_ACTOR_ID 0xFFFFFFFF
#define DEFAULT_ACTOR_NAME "actor"

// class Geo_Actor;
// struct Topl_EntryShader;

typedef void (*pickerCallback)(MOUSE_Event);
typedef void (*shaderCallback)(void);

// typedef void (*pickerCallback)(Geo_Actor*, MOUSE_Event);
// typedef void (*shaderCallback)(Geo_Actor*, Topl_EntryShader*);

class Geo_Actor {
public:
	Geo_Actor(){
#ifdef RASTERON_H
		_id = color_unique();
#endif
	}
	Geo_Actor(const Geo_Mesh* mesh) {
#ifdef RASTERON_H
		_id = color_unique();
#endif
		_mesh = mesh;
	}
	Geo_Actor(const std::string& name, const Geo_Mesh* mesh) {
#ifdef RASTERON_H
		_id = color_unique();
#endif
		_name = name;
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
	void copyParams(const Geo_Actor& actor){
		setPos(*actor.getPos());
		setRot(*actor.getRot());
		setSize(*actor.getSize());
	}

	unsigned getId() const { return _id; }
	std::string getName() const { return _name; }
	std::string getNameExt() const {
		std::string ext = "";
		for(unsigned short e = _name.length() - 1; e > 0 && isdigit(_name[e]); e--)
			ext += _name[e];
		std::reverse(ext.begin(), ext.end());
		return (ext.length() > 0)? ext : "0";
	}
    std::string toString() const { return _name + ":I=" + std::to_string(_id) + ":P=" + _position.toString() + ":R=" + _rotation.toString() + ":S=" + _scale.toString(); }
	const Geo_Mesh* getMesh() const { return _mesh; }
	vec3f_cptr_t getPos() const { return &_position; }
	vec3f_cptr_t getRot() const { return &_rotation; }
	vec3f_cptr_t getSize() const { return &_scale; }

	bool isShown = true;
	// pickerCallback pickerFunc = nullptr; // invoked on color picking operation
	shaderCallback shaderFunc = nullptr; // invoked during shader render block

	std::function<void(MOUSE_Event event, Geo_Actor* actor)> pickFunc;
private:
	// Data types
	unsigned _id = rand() % (256 * 256); // id used for color picking
	std::string _name = DEFAULT_ACTOR_NAME; // user-friendly name
	const Geo_Mesh* _mesh = nullptr; // mesh stored internally

	// Orientation types
    Vec3f _position = VEC_3F_ZERO;
    Vec3f _rotation = VEC_3F_ZERO;
    Vec3f _scale = VEC_3F_ONES;
};

typedef const Geo_Actor* const actor_cptr;

#define GEO_ACTOR_H
#endif
