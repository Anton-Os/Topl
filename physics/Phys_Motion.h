#ifndef PHYS_MOTION_H

#include "maths.h"

// Motion

#define MOTION_NO_CURVE 0.0f // no alterations
#define MOTION_IN_CURVE 1.0f // x2 towards center and 1/2 away (convergance)
#define MOTION_OUT_CURVE -1.0f // 1/2 towards center and x2 away (divergance)

enum MOTION_Type {
	MOTION_Instant,
	MOTION_Linear,
	MOTION_Orbit,
};

class Phys_Motion {
public:
    Phys_Motion(MOTION_Type t, Vec3f m, double d){ // Forward and Reverse Motion
		type = t;
		pos1 = m; pos2 = m * -1.0f;
        duration = d;
    }
    Phys_Motion(MOTION_Type t, Vec3f startVec, Vec3f endVec, double d){ // Start and End Motion
		type = t;
		pos1 = startVec; pos2 = endVec;
        duration = d;
    }
    Vec3f getMotion(double currentSecs){
        seqCount = static_cast<unsigned>(std::floor(getSeqProg(currentSecs)));
        double seqProg = getSeqProg(currentSecs) - seqCount; // computes what fraction of sequence elapsed (0.0 to 1.0)
        // TODO: implement time change based on specified curve

		switch (type) {
		case MOTION_Instant: return (seqProg <= 0.5)? pos1 : pos2;
		case MOTION_Linear : return getLinear(pos1, pos2, seqProg);
		case MOTION_Orbit: return VEC_3F_ZERO; // TODO: implement orbit motion!
		}
    }
    void setCurve(double c){ curve = c; }

private:
    double getSeqProg(double currentSecs){
		while (currentSecs > duration) currentSecs -= duration;
		return currentSecs / duration; 
	} // gets progress in sequence
    Vec3f getLinear(const Vec3f& startVec, const Vec3f& endVec, double progFrac){ // linear motion computation
        return (progFrac <= 0.5f)
			? Vec3f(startVec) + (Vec3f(endVec) * (float)(progFrac * 2.0f)) // forward motion
			: Vec3f(endVec) + (Vec3f(startVec) * (float)(progFrac * 2.0f)); // reverse motion
    }

	MOTION_Type type;
    Vec3f pos1 = VEC_3F_ZERO; // start motion vector
    Vec3f pos2 = VEC_3F_ZERO; // end motion vector
    double curve = MOTION_NO_CURVE;

	double duration = 0.0; // duration of motion
    unsigned seqCount = 0; // tracks iterations of motion
};

#define PHYS_MOTION_H
#endif