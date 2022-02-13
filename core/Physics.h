#include <cstdlib>
#include <cmath>
#include <vector>

#include <Eigen/Dense>

#define MOTION_NO_CURVE 0.0f // curve has no alterations
#define MOTION_IN_CURVE 1.0f // curve is x2 towards center and 1/2 away
#define MOTION_OUT_CURVE 1.0f // curve is 1/2 towards center and x2 away

enum MOTION_Type {
	MOTION_Instant,
	MOTION_Linear,
	// MOTION_Smooth,
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
		// case MOTION_Smooth: return Eigen::Vector3f(0.0, 0.0, 0.0); // implement smooth motion!
		case MOTION_Pivot: return Eigen::Vector3f(0.0, 0.0, 0.0); // implement pivot motion!
		}
    }
    void setCurve(double c){ motionCurve = c; }

private:
    double getSeqProg(double currentSecs){ return currentSecs / (endSecs - startSecs); } // gets progress in sequence
    Eigen::Vector3f getLinear(const Eigen::Vector3f& m1, const Eigen::Vector3f& m2, double progFrac){ // linear motion computation
        // TODO: implement curve logic here!
        return (progFrac <= 0.5f)? m1 + (m2 * progFrac * 2.0f) : m2 + (m1 * progFrac * 2.0f);
    }

	MOTION_Type type;
    Eigen::Vector3f startPos = Eigen::Vector3f(0.0, 0.0, 0.0); // start motion vector
    Eigen::Vector3f endPos = Eigen::Vector3f(0.0, 0.0, 0.0); // end motion vector
    double motionCurve = MOTION_NO_CURVE; // no modification of time argument

    double startSecs = 0.0f;
    double endSecs = 0.0f;
    unsigned seqCount = 0; // tracks how many sequence iterations have occured
    bool isFirstCall = true; // required to correct the timekeeping
};

#define PHYS_DEFAULT_K 2000.0 // K value by default
#define PHYS_ROD_K 1000000.0 // K value for rod connector
#define CONNECTOR_LEN_THRESH 0.00005 // Threshold value for spring oscillations
#define CONNECTOR_ANGLE_THRESH Eigen::Vector3f(0.00005f, 0.00005f, 0.00005f)
#define CONNECTOR_ANGLE_SCALE 20
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
    // Phys_Connector(double l, double rl, double a, CONNECT_Type t, double k){
    Phys_Connector(double l, double k){ // Basic Constructor
        type = CONNECT_Spring;
        length = l;
        restLength = l;
        kVal = k;
    }
    Phys_Connector(CONNECT_Type t, double l, double k){ // Extended Constructor
        type = t;
        length = l;
        restLength = l;
        kVal = (type != CONNECT_Rod)? k : PHYS_ROD_K;
    }
    bool getIsPreset(){ // determines whether internals are correctly set
       return (length == BAD_CONNECTOR_LEN || restLength == BAD_CONNECTOR_LEN)? false : true;
    }

    // Updatable 
	Eigen::Vector3f centerPoint = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    Eigen::Vector3f restAngle_NVec1, restAngle_NVec2; // AT REST normalized vectors pointing to 1st and 2nd links
    Eigen::Vector3f angle_NVec1, angle_NVec2; // CURRENT normalized vectors pointing to 1st and 2nd links

    CONNECT_Type type = CONNECT_Spring;
    double length = BAD_CONNECTOR_LEN; // current length of the connector
    double restLength = BAD_CONNECTOR_LEN; // tries to reach this rest length
	double kVal = PHYS_DEFAULT_K; // spring stiffness known as k constant
};

#define BARRIER_DIM 1.0f // default barrier dimention
#define BARRIER_EXPAND 10000.0f // barrier infinite expanse

struct Phys_Barrier {
    Phys_Barrier(Eigen::Vector3f p){ pos = p; } // Positioned Constructor
    Phys_Barrier(Eigen::Vector3f p, float w, float h, float d){ // Extended Constructor
        pos = p;

        width = w;
        height = h;
        depth = d;
    }
    Eigen::Vector3f pos;

    float height = BARRIER_DIM;
    float width = BARRIER_DIM * 3;
    float depth = BARRIER_EXPAND;
};

struct Phys_Constraint { // Limits an objects freedom of motion
    std::vector<const Phys_Barrier*> rigidBarrs;
    std::vector<std::pair<Eigen::Vector2f, const Phys_Barrier*>> angleBarrs;

    // Include other constraints as necessary
};

#define PHYS_FORCE_UNIT 0.05 // Easy unit to work in screen coordinates
#define PHYS_DEFAULT_DAMPING 1 // 1 For testing, no slow down
#define PHYS_DEFAULT_MASS 15.0 // default mass of entities
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
    void addConstraint(const Phys_Barrier* barrier){
        constraints.rigidBarrs.push_back(barrier);
    }
    void addConstraint(Eigen::Vector2f angles, const Phys_Barrier* barrier){
        constraints.angleBarrs.push_back(std::make_pair(angles, barrier));
    }

    Phys_Constraint constraints; // use this to limit freedom of motion

    bool isGravityEnabled = false;
	const double damping = PHYS_DEFAULT_DAMPING;
    double mass = PHYS_DEFAULT_MASS;

	Eigen::Vector3f velocity = Eigen::Vector3f(0.0, 0.0, 0.0);
    Eigen::Vector3f acceleration = Eigen::Vector3f(0.0, 0.0, 0.0);
    unsigned short actingForceCount = 0; // number of acting forces on theobject
    Eigen::Vector3f* forces = nullptr;
    // const Phys_Colliders* colliders = nullptr; // tracks colliding bodies for collision detection
};

