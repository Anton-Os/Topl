#include <cstdlib>
#include <cmath>
#include <vector>

#include <Eigen/Dense>

// TODO: ADD DEFINITIONS TO SCALE PHYSICS REACTIONS

#define TOPL_FORCE_UNIT 0.04 // Easy unit to work in screen coordinates
#define TOPL_DEFAULT_DAMPING 1 // 1 For Testing Purpose, no slow down
#define TOPL_DEFAULT_MASS 15.0 // Default Mass of Entities
#define TOPL_DEFAULT_K 3000.0 // K Value for Springs
#define TOPL_CONNECTOR_LEN_THRESH 0.00005 // Threshold value for spring oscillations
#define TOPL_CONNECTOR_ANGLE_THRESH Eigen::Vector3f(0.00005f, 0.00005f, 0.00005f)
#define TOPL_CONNECTOR_ANGLE_MULT 20

enum MOTION_Type {
	MOTION_Instant,
	MOTION_Linear,
	MOTION_Smooth,
	MOTION_Pivot,
};

class Phys_Motion { // Motion can be used for forces, absolute position updates, and even rotations!
public:
    Phys_Motion(MOTION_Type t, Eigen::Vector3f m, double d){ // Motion with Counter-Movement Constructor
		type = t;
		startPos = m;
        endPos = -1.0f * m;

        endSecs = d;
    }
    Phys_Motion(MOTION_Type t, Eigen::Vector3f m1, Eigen::Vector3f m2, double d){ // Start and End Motion Vec
		type = t;
		startPos = m1;
		endPos = m2;

        endSecs = d;
    }
    Eigen::Vector3f getMotion(double currentSecs){
        if(isFirstCall){
            startSecs += currentSecs;
            endSecs += currentSecs;
            isFirstCall = false;
        }

        seqCount = static_cast<unsigned>(std::floor(getSeqProg(currentSecs)));
        double seqProgFrac = getSeqProg(currentSecs) - seqCount; // computes what fraction of sequence elapsed (0.0 to 1.0)

		switch (type) {
		case MOTION_Instant: return (seqProgFrac <= 0.5f)? startPos : endPos;
		case MOTION_Linear : return getLinear(startPos, endPos, seqProgFrac);
		case MOTION_Smooth: return Eigen::Vector3f(0.0, 0.0, 0.0); // implement smooth motion!
		case MOTION_Pivot: return Eigen::Vector3f(0.0, 0.0, 0.0); // implement pivot motion!
		}
    }

private:
    double getSeqProg(double currentSecs){ return currentSecs / (endSecs - startSecs); } // gets progress in sequence
    Eigen::Vector3f getLinear(const Eigen::Vector3f& m1, const Eigen::Vector3f& m2, double progFrac){ // linear motion computation
        return (progFrac <= 0.5f)? m1 + (m2 * progFrac) : m2 + (m1 * progFrac);
    }

	MOTION_Type type;
    Eigen::Vector3f startPos = Eigen::Vector3f(0.0, 0.0, 0.0); // start motion vector
    Eigen::Vector3f endPos = Eigen::Vector3f(0.0, 0.0, 0.0); // end motion vector

    double startSecs = 0.0f;
    double endSecs = 0.0f;
    unsigned seqCount = 0; // tracks how many sequence iterations have occured
    bool isFirstCall = true; // required to correct the timekeeping
};

enum CONNECT_Type {
    CONNECT_Spring, // Oscillating force both ways
    CONNECT_Rod, // Fixed distance apart
    // CONNECT_Bungee, // Exerts pulling force, no push
    // CONNECT_String, // Fixed maximum distance apart
    // CONNECT_Cable // Fixed min and max distance apart
};

struct Phys_Connector {
    Phys_Connector(){ }
    Phys_Connector(double l, double rl, double a, CONNECT_Type t, double k){
        length = l;
        restLength = rl;
        // angle = a;
        type = t;
        kVal = k;
    }

    // Updatable 
	Eigen::Vector3f centerPoint = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    Eigen::Vector3f restAngleNormVec1, restAngleNormVec2; // AT REST normalized vectors that point to 1st and 2nd linked items
    Eigen::Vector3f angleNormVec1, angleNormVec2; // CURRENT normalized vectors that point to 1st and 2nd linked items

    CONNECT_Type type = CONNECT_Spring;
    double length = 0.5f; // tries to reach rest length from here
    double restLength = 0.5f; // zero forces act at this length
	double kVal = TOPL_DEFAULT_K; // 100.0 seems to be normal
};

#define MAX_PHYS_FORCES 64

struct Phys_Actor { // A physics property that becomes associated to a Geo_Actor object
    Phys_Actor(){ // Freeform constructor
        forces = (Eigen::Vector3f*)malloc(MAX_PHYS_FORCES * sizeof(Eigen::Vector3f));
    }
    Phys_Actor(Eigen::Vector3f gravityVec){ // Gravity constructor
        forces = (Eigen::Vector3f*)malloc(MAX_PHYS_FORCES * sizeof(Eigen::Vector3f));
        
        addForce(gravityVec);
        isGravityEnabled = true;
    }
    ~Phys_Actor(){ if(forces != nullptr) free(forces); }

    bool addForce(Eigen::Vector3f force){
        if(actingForceCount < MAX_PHYS_FORCES){
            *(forces + actingForceCount) = force;
            actingForceCount++;
            return true;
        } else return false;
    }

    bool isGravityEnabled = false;
	const double damping = TOPL_DEFAULT_DAMPING;
    double mass = TOPL_DEFAULT_MASS;

	Eigen::Vector3f velocity = Eigen::Vector3f(0.0, 0.0, 0.0);
    Eigen::Vector3f acceleration = Eigen::Vector3f(0.0, 0.0, 0.0);

    unsigned short actingForceCount = 0; // number of acting forces on theobject
    Eigen::Vector3f* forces = nullptr;
    // const Phys_Colliders* colliders = nullptr; // tracks colliding bodies for collision detection
};

