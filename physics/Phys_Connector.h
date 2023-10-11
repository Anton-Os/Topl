#ifndef PHYS_CONNECTOR_H

#include "Geo_Actor.hpp"

#define PHYS_DEFAULT_K 1000.0 // K value by default
#define PHYS_ROD_K 10000.0 // K value for rod connector
#define CONNECTOR_LEN_THRESH 0.00005 // Threshold value for spring oscillations
// #define CONNECTOR_ANGLE_THRESH Vec3f(0.00005f, 0.00005f, 0.00005f)
#define CONNECTOR_ANGLE_PRESET Vec3f({ 0.0f, 1.0f, 0.0 });
#define CONNECTOR_ANGLE_SCALE 20.0f
#define BAD_CONNECTOR_LEN -1.0f // indicates that parameters need to be adjusted

enum CONNECT_Type {
    CONNECT_Spring, // Oscillating force both ways
    CONNECT_Rod, // Fixed distance apart
    // CONNECT_Bungee, // Exerts pulling force, no push
    // CONNECT_String, // Fixed maximum distance apart
    // CONNECT_Cable // Fixed min and max distance apart
};

struct Phys_Connector {
    Phys_Connector(){ } // Empty Constructor
    Phys_Connector(double l, double k){ // Basic Constructor
        length = l;
        restLength = l;
        kVal = k;
    }

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
    bool getIsPreset(){ // determines whether internals are correctly set
       return (length == BAD_CONNECTOR_LEN || restLength == BAD_CONNECTOR_LEN)? false : true;
    }

    // Updatable 
	Vec3f centerPoint = VEC_3F_ZERO;
    Vec3f restAngleVecN1 = CONNECTOR_ANGLE_PRESET; // pos1 tries to stabilize to rest angle
    Vec3f restAngleVecN2 = restAngleVecN1.inverse(); // pos2 tries to stabilize to rest angle
    Vec3f angleVecN1 = CONNECTOR_ANGLE_PRESET; // current angle towards pos1
    Vec3f angleVecN2 = angleVecN1.inverse(); // current angle towards pos2

    double length = BAD_CONNECTOR_LEN; // current length of the connector
    double restLength = BAD_CONNECTOR_LEN; // tries to reach this rest length
	double kVal = PHYS_DEFAULT_K; // spring stiffness known as k constant
};

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