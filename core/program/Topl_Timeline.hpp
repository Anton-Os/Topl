#ifndef TOPL_TIMELINE_H

#include <map>

#include "Timer.hpp"
#include "maths/Vector.h"

// #define MAX_TIMELINE_ATTRIBS 2056
#define TIMELINE_START 0.0 // 0 millisecs will always be start
#define TIMELINE_AT -1.0
#define TIMELINE_END 60.0 // 1 minute will be default end
#define TIMELINE_FORETELL -0.5

class Topl_Timeline { // TODO: Include audio capabilities
public:
	Topl_Timeline(){}

	template<typename T> void addSequence(T var, std::pair<millisec_t, T> target);
	// template<class T> void addSequence(T var, std::pair<millisec_t, T> target); // try to templatize this
	void addSequence(Vec3f* vec, std::pair<millisec_t, Vec3f> target);
	void addPeriodic(Vec3f* vec, std::pair<millisec_t, Vec3f> target, unsigned short reps); // the target will oscillate between two values 
	void addSequence(float* var, std::pair<millisec_t, float> target);
	void addPeriodic(float* var, std::pair<millisec_t, float> target, unsigned short reps); // the target will oscillate between two values 
	void addSequence(double* var, std::pair<millisec_t, double> target);

	Timer_Dynamic dynamic_ticker = Timer_Dynamic(TIMELINE_START); // variably incrementing
	Timer_Persist persist_ticker; // constantly incrementing

	template<typename T> void updateSequence(T, millisec_t);
	static void seqCallback(millisec_t m);
private:
	static std::map<Vec3f*, std::map<millisec_t, Vec3f>> vec3f_map;
	static std::map<float*, std::map<millisec_t, float>> float_map;
	static std::map<double*, std::map<millisec_t, double>> double_map;

	std::vector<Vec3f> vec3f_data;
	std::vector<float> float_data;
	std::vector<double> double_data;
};

#define TOPL_TIMELINE_H
#endif