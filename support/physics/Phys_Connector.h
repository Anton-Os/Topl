#ifndef PHYS_CONNECTOR_H

#include "Geo_Actor.hpp"

#define PHYS_DEFAULT_K 5000.0 // K value by default
#define PHYS_ROD_K 10000.0 // K value for rod connector
#define PHYS_CALC_THRESH 0.05 // time threshhold for adding forces
#define CONNECTOR_LEN_THRESH 0.00005 // Threshold value for spring oscillations
// #define CONNECTOR_ANGLE_THRESH Vec3f(0.00005f, 0.00005f, 0.00005f)
#define CONNECTOR_ANGLE_PRESET Vec3f({ 0.0f, 1.0f, 0.0 });
#define CONNECTOR_ANGLE_SCALE 20.0f

struct Phys_Connector {
    Phys_Connector(){ } // Empty Constructor
    Phys_Connector(const Vec3f& pos1, const Vec3f& pos2){ preset(pos1, pos2); } // Preset Constructor

    void preset(const Vec3f& pos1, const Vec3f& pos2){ // presets the connector values given positions
        Vec3f diffVec = Vec3f(pos1) - Vec3f(pos2);
        length = diffVec.len(); // presets length between points
        restLength = length; // rest length set to default length
        centerPoint = (Vec3f(pos1) + Vec3f(pos2)) * 0.5f; // midpoint equation

        restAngleVecN1 = Vec3f(pos1) - centerPoint; restAngleVecN1.normalize(); // computes the norm vector towards pos1
        angleVecN1 = restAngleVecN1; // initially set to rest angle
        restAngleVecN2 = Vec3f(pos2) - centerPoint; restAngleVecN2.normalize(); // computes the norm vector towards pos2
        angleVecN2 = restAngleVecN2; // sets current angle to the rest angle
    }

    // Updatable 
	Vec3f centerPoint = VEC_3F_ZERO;
    Vec3f restAngleVecN1 = CONNECTOR_ANGLE_PRESET; // pos1 tries to stabilize to rest angle
    Vec3f restAngleVecN2 = restAngleVecN1.inverse(); // pos2 tries to stabilize to rest angle
    Vec3f angleVecN1 = CONNECTOR_ANGLE_PRESET; // current angle towards pos1
    Vec3f angleVecN2 = angleVecN1.inverse(); // current angle towards pos2

    double length = 1.0; // current length of the connector
    double restLength = 1.0; // tries to reach this rest length
	double kVal = PHYS_DEFAULT_K; // spring stiffness known as k constant
};

// struct Phys_Spring : Phys_Connector {} // loose
// struct Phys_Rod : Phys_Connector {} // rigid

// Links and Anchors

typedef std::pair<const Geo_Actor*, const Geo_Actor*> link_pair;
typedef std::pair<const Geo_Actor*, const Vec3f*> anchor_pair;

struct LinkedItems { // wrapper around connector with 2 actors being linked
	Phys_Connector* connector;
	link_pair linkedItems;
};
typedef const LinkedItems* const linkedItems_cptr;

struct AnchoredItems { // Wrapper around connector with 1 actor being anchored
	Phys_Connector* connector;
	anchor_pair anchoredItems;
};
typedef const AnchoredItems* const anchorItem_cptr;

#define PHYS_CONNECTOR_H
#endif