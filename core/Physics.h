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

/* enum MOTION_Type { MOTION_Linear }; */

class Phys_Motion { // Motion can be used for forces, absolute position updates, and even rotations!
public:
    Phys_Motion(Eigen::Vector3f m, double d){ // Motion with Counter-Movement Constructor
        startMotionVec = m;
        endMotionVec = -1.0 * m;
        endSecs = d;
    }

    Phys_Motion(Eigen::Vector3f sm, Eigen::Vector3f em, double d){ // Start and End Motion Vec
        startMotionVec = sm;
        endMotionVec = em;
        endSecs = d;
    }

    Eigen::Vector3f getMotion(double currentSecs){
        if(isFirstCall){
            startSecs += currentSecs;
            endSecs += currentSecs;
            isFirstCall = false; // makes sure this segment doesnt repeat
        }

        seqCount = static_cast<unsigned>(std::floor(getSeqProgress(currentSecs)));
        double seqProgressFrac = getSeqProgress(currentSecs) - seqCount; // gets what fraction of the sequence has elapsed (0.0 to 1.0)
        Eigen::Vector3f motionVec = (seqProgressFrac < 0.5f) ? startMotionVec : endMotionVec; // TODO: set proper values!

        return motionVec; // TODO: set proper values!
    }

private:
    double getSeqProgress(double currentSecs){ return currentSecs / (endSecs - startSecs); } // gets the progression in the sequence

    // MOTION_Type motionType = MOTION_Linear;
    double startSecs = 0.0f;
    double endSecs = 0.0f;
    unsigned maxIter = 0; // by default repeats infinitely
    unsigned seqCount = 0;
    bool isFirstCall = true; // set to false after first call to get position

    Eigen::Vector3f startMotionVec; // start motion vector
    Eigen::Vector3f endMotionVec; // end motion vector
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

    CONNECT_Type type = CONNECT_Rod;
    double length = 0.5f; // tries to reach rest length from here
    double restLength = 0.5f; // zero forces act at this length
	double kVal = TOPL_DEFAULT_K; // 100.0 seems to be normal
};

#define MAX_PHYS_FORCES 64

struct Phys_Colliders {
    Phys_Colliders(){}
    Phys_Colliders(const Eigen::Vector3f* o){ origin = o; }

    float radii = 0.0f; // all colliding spheres that are uniform in size
    std::vector<Eigen::Vector3f> offsets; // offsets from origin of colliders
    const Eigen::Vector3f* origin = nullptr; // origin of associated geometry object
};

struct Phys_Properties { // A physics property that becomes associated to a Geo_Component object
    Phys_Properties(){ // Freeform constructor
        forces = (Eigen::Vector3f*)malloc(MAX_PHYS_FORCES * sizeof(Eigen::Vector3f));
    }
    Phys_Properties(Eigen::Vector3f gravityVec){ // Gravity constructor
        forces = (Eigen::Vector3f*)malloc(MAX_PHYS_FORCES * sizeof(Eigen::Vector3f));
        
        addForce(gravityVec);
        isGravityEnabled = true;
    }
    ~Phys_Properties(){ if(forces != nullptr) free(forces); }

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
    const Phys_Colliders* colliders = nullptr; // tracks colliding bodies for collision detection
};