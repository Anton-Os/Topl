#ifndef PHYS_H

#include <cstdlib>

#include "maths.h"

// Physics Actor

#define PHYS_FORCE_UNIT 0.05 // Easy unit to work in screen coordinates
// #define PHYS_DEFAULT_DAMPING 1 // 1 For testing, no slow down
#define PHYS_DEFAULT_DAMPING 0.98
#define PHYS_DEFAULT_MASS 10.0 // default mass of entities
#define MAX_PHYS_FORCES 64

struct Phys_Actor { // A physics property that becomes associated to a Geo_Actor object
	Phys_Actor() { // Freeform constructor
		forces = (Vec3f*)malloc(MAX_PHYS_FORCES * sizeof(Vec3f));
	}
    Phys_Actor(double m){
        mass = m;
        forces = (Vec3f*)malloc(MAX_PHYS_FORCES * sizeof(Vec3f));
    }
	/* Phys_Actor(Vec3f gravityVec) { // Gravity constructor
		forces = (Vec3f*)malloc(MAX_PHYS_FORCES * sizeof(Vec3f));

		addForce(gravityVec);
		isGravityEnabled = true;
	} */
	~Phys_Actor() { if (forces != nullptr) free(forces); }

	bool addForce(Vec3f force) {
		if (actingForceCount < MAX_PHYS_FORCES) {
			*(forces + actingForceCount) = force;
			actingForceCount++;
			return true;
		}
		else return false;
	}

	bool isGravityEnabled = false;
	double damping = PHYS_DEFAULT_DAMPING;
	double mass = PHYS_DEFAULT_MASS;

	Vec3f velocity = VEC_3F_ZERO;
	Vec3f acceleration = VEC_3F_ZERO;

	Vec3f* forces = nullptr;
	unsigned short actingForceCount = 0; // number of acting forces on the object
};

#define PHYS_H
#endif