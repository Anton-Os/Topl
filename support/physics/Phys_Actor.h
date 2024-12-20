#ifndef PHYS_H

#include <cstdlib>

#include "maths.h"

// Physics Force

// #define PHYS_DEFAULT_DAMPING 1 // 1 For testing, no slow down
#define PHYS_DEFAULT_DAMPING 0.98
#define PHYS_DEFAULT_MASS 10.0 // default mass of entities
#define MAX_PHYS_FORCES 64

enum FORCE_Type { FORCE_Directional, FORCE_Angular, FORCE_Constricting };

struct Phys_Force {
    Phys_Force(Vec3f f){ force = f; }
    Phys_Force(FORCE_Type t, Vec3f f){
        type = t;
        force = f;
    }

    FORCE_Type type = FORCE_Directional;
    Vec3f force;
};

// Physics Actor

struct Phys_Actor { // A physics property that becomes associated to a Geo_Actor object
	Phys_Actor() { // Freeform constructor
        actingForces = (Phys_Force*)malloc(MAX_PHYS_FORCES * sizeof(Phys_Force));
	}
    Phys_Actor(double m){
        mass = m;
        actingForces = (Phys_Force*)malloc(MAX_PHYS_FORCES * sizeof(Phys_Force));
    }
	/* Phys_Actor(Vec3f gravityVec) { // Gravity constructor
		forces = (Vec3f*)malloc(MAX_PHYS_FORCES * sizeof(Vec3f));

		addForce(gravityVec);
		isGravityEnabled = true;
	} */
    ~Phys_Actor() { if (actingForces != nullptr) free(actingForces); }

    void addForce(Vec3f forceVec) {
		if (actingForceCount < MAX_PHYS_FORCES) {
            *(actingForces + actingForceCount) = Phys_Force(forceVec);
            actingForceCount++;
        } else logMessage(MESSAGE_Exclaim, "Forces excess!");
	}

    void addForce(FORCE_Type type, Vec3f forceVec) {
        if (actingForceCount < MAX_PHYS_FORCES) {
            *(actingForces + actingForceCount) = Phys_Force(type, forceVec);
            actingForceCount++;
        } else logMessage(MESSAGE_Exclaim, "Forces excess!");
    }

    Vec3f getForceVecAt(unsigned f){ return (*(actingForces + f)).force; }

    // bool isGravityEnabled = false;
	double damping = PHYS_DEFAULT_DAMPING;
	double mass = PHYS_DEFAULT_MASS;

    Vec3f velocity = VEC_3F_ZERO, acceleration = VEC_3F_ZERO;
    Vec3f angle = VEC_3F_ZERO, angularVelocity = VEC_3F_ZERO, angularAcceleration = VEC_3F_ZERO;
    Vec3f scale = VEC_3F_ZERO, scaleVelocity = VEC_3F_ZERO, scaleAcceleration = VEC_3F_ZERO;

    Phys_Force* actingForces = nullptr;
	unsigned short actingForceCount = 0; // number of acting forces on the object
};

#define PHYS_H
#endif
