#include <vector>

#include <Eigen/Dense>

#include "Timer.hpp"

// void iterateDynamicNodes(std::vector<Topl_GeoNode*>& dynNodes);

/* class Physics_Force {
private:
    std::vector<updatePos_abs> absMovs; // absolute movements
    std::vector<updatePos_timed> timedMovs;

    Eigen::Vector3f mPos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    // std::vector<Eigen::Vector3f> mForces; 
}; */

class Physics_Move {
public:
    Physics_Move(){}
    //virtual void updatePos();
    Eigen::Vector3f getPos(){ return mPos; }
protected:
    Eigen::Vector3f mPos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
};

typedef Eigen::Vector3f (*updatePos_abs)(const Eigen::Vector3f& pos);

class Physics_MoveAbs : Physics_Move {
public:
    Physics_MoveAbs(updatePos_abs updateFunc) : Physics_Move() {
        mAbsMove = updateFunc;
    }
	void updatePos(int reps);
private:
    updatePos_abs mAbsMove;
	int mActionRepeats; // Negative moves in other direction
};

typedef Eigen::Vector3f (*updatePos_timed)(const Eigen::Vector3f& pos, const Timer_EventRatio& timeEvent);

class Physics_MoveTimed : Physics_Move {
public:
    Physics_MoveTimed(updatePos_timed updateFunc) : Physics_Move() {
        mTimedMove = updateFunc;
    }
	void updatePos(const Timer_EventRatio& time);
private:
    updatePos_timed mTimedMove;
	Timer_EventRatio mActionTimes;
};