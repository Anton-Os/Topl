#include <vector>

#include <Eigen/Dense>

#include "Timer.hpp"

typedef Eigen::Vector3f (*updatePos_abs)(const Eigen::Vector3f& pos);
typedef Eigen::Vector3f(*updatePos_timed)(const Eigen::Vector3f& pos, Timer_Event* tEvent);

// void iterateDynamicNodes(std::vector<Topl_GeoEntity*>& dynNodes);

class Physics_Force {
private:
    Eigen::Vector3f mPos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    std::vector<Eigen::Vector3f> mForces; 
};