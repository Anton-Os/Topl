#include <cstdlib>
#include <vector>

#include <Eigen/Dense>

enum CONNECT_Type {
    CONNECT_Spring, // Oscillating force both ways
    CONNECT_Bungee, // Exerts pulling force, no push
    CONNECT_String, // Fixed maximum distance apart
    CONNECT_Rod, // Fixed distance apart
    CONNECT_Cable // Fixed min and max distance apart
};

struct Phys_Connector {
    CONNECT_Type type= CONNECT_Rod;
    double restLength = 0.5f;
	double kVal = 0.88f; // Probably needs to be a constant
    // double deviation = 0.1f;
};

#define MAX_PHYS_FORCES 64

struct Phys_Properties { // This binds to a Topl_GeoNode
    Phys_Properties(){
        forces = (Eigen::Vector3f*)malloc(MAX_PHYS_FORCES * sizeof(Eigen::Vector3f));
        // Add the gravity force here, it will always be acting on the body
    }
    ~Phys_Properties(){ if(forces != nullptr) free(forces); }

	const double damping = 0.98f;
    double mass = 1.0;

	Eigen::Vector3f velocity = Eigen::Vector3f(0.0, 0.0, 0.0);
    Eigen::Vector3f acceleration = Eigen::Vector3f(0.0, 0.0, 0.0);

    unsigned actingForceCount = 0; // Indicates the gravity force
    Eigen::Vector3f* forces = nullptr;
};