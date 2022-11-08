#ifndef PHYS_MOTION_H

#include "maths.h"

// Motion

#define MOTION_NO_CURVE 0.0f // curve has no alterations
#define MOTION_IN_CURVE 1.0f // curve is x2 towards center and 1/2 away (convergance)
#define MOTION_OUT_CURVE -1.0f // curve is 1/2 towards center and x2 away (divergance)

enum MOTION_Type {
	MOTION_Instant,
	MOTION_Linear,
	MOTION_Orbit,
};

class Phys_Motion { // Motion can be used for forces, absolute position updates, and even rotations!
public:
    Phys_Motion(MOTION_Type t, Vec3f m, double d){ // Motion with Counter-Movement Constructor
		type = t;
		pos1 = m; pos2 = m * -1.0f;
        endSecs = d;
    }
    Phys_Motion(MOTION_Type t, Vec3f m1, Vec3f m2, double d){ // Start and End Motion Vec
		type = t;
		pos1 = m1; pos2 = m2;
        endSecs = d;
    }
    Vec3f getMotion(double currentSecs){
        if(isFirstCall){
            startSecs += currentSecs;
            endSecs += currentSecs;
            isFirstCall = false;
        }

        seqCount = static_cast<unsigned>(std::floor(getSeqProg(currentSecs)));
        double seqProg = getSeqProg(currentSecs) - seqCount; // computes what fraction of sequence elapsed (0.0 to 1.0)
        if(curve != MOTION_NO_CURVE && seqProg != 0.0 && seqProg != 1.0){
            // double seqMod = abs(seqProg - 0.5f);
            // seqProg += seqMod;
        }

		switch (type) {
		case MOTION_Instant: return (seqProg <= 0.5)? pos1 : pos2;
		case MOTION_Linear : return getLinear(pos1, pos2, seqProg);
		// case MOTION_Orbit: return VEC_3F_ZERO; // implement pivot motion!
		}
    }
    void setCurve(double c){ curve = c; }

private:
    double getSeqProg(double currentSecs){ return currentSecs / (endSecs - startSecs); } // gets progress in sequence
    Vec3f getLinear(const Vec3f& m1, const Vec3f& m2, double progFrac){ // linear motion computation
        return (progFrac <= 0.5f)
			? Vec3f(m1) + (Vec3f(m2) * (float)(progFrac * 2.0f)) // forward motion
			: Vec3f(m2) + (Vec3f(m1) * (float)(progFrac * 2.0f)); // reverse motion
    }

	MOTION_Type type;
    Vec3f pos1 = VEC_3F_ZERO; // start motion vector
    Vec3f pos2 = VEC_3F_ZERO; // end motion vector
    double curve = MOTION_NO_CURVE; // no curve by default

    double startSecs = 0.0f;
    double endSecs = 0.0f;
    unsigned seqCount = 0; // tracks how many sequence iterations have occured
    bool isFirstCall = true; // required to correct the timekeeping
};

#define PHYS_MOTION_H
#endif