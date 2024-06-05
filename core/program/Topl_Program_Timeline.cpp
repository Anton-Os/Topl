#include "program/Topl_Program.hpp"


std::map<Vec3f*, std::map<millisec_t, Vec3f>> Topl_Timeline::vec3f_map = std::map<Vec3f*, std::map<millisec_t, Vec3f>>();
std::map<float*, std::map<millisec_t, float>> Topl_Timeline::float_map = std::map<float*, std::map<millisec_t, float>>();
std::map<double*, std::map<millisec_t, double>> Topl_Timeline::double_map = std::map<double*, std::map<millisec_t, double>>();

template<typename T> 
void Topl_Timeline::updateSequence(T map, millisec_t time){
	for(auto f = map.begin(); f != map.end(); f++){
		auto seq_start = f->second.begin();
		auto seq_end = f->second.rbegin();

		// TODO: iterate through map and determine new value
	}
}

void Topl_Timeline::seqCallback(double m){
	for(auto f = Topl_Timeline::vec3f_map.begin(); f != vec3f_map.end(); f++){
		std::map<millisec_t, Vec3f>::iterator seq_start = f->second.begin();
		std::map<millisec_t, Vec3f>::reverse_iterator seq_end = f->second.rbegin();

		if(f->second.size() > 2){ // std::cout << "2+ range detected" << std::endl;
			auto b = std::next(seq_start);
			while(b != f->second.end()) if(m > b->first){ seq_start = b; b++; } else break;
			auto t = std::next(seq_end);
			while(t != f->second.rend()) if(m < t->first){ seq_end = t; t++; } else break;
		}

		double prog =  (m - seq_start->first) / (seq_end->first - seq_start->first);

		if(m > seq_end->first) *(f->first) = seq_end->second; // went over the time limit
		else if(m < seq_start->first) *(f->first) = seq_start->second;
		else *(f->first) = seq_start->second + ((seq_end->second - seq_start->second) * prog);
		// else *(f->first) = r->second + ((m - r->first) * ((s->second - r->second) / (s->first - r->first)));
	}

	for(auto f = Topl_Timeline::float_map.begin(); f != float_map.end(); f++){
		std::map<millisec_t, float>::iterator seq_start = f->second.begin();
		std::map<millisec_t, float>::reverse_iterator seq_end = f->second.rbegin();

		if(f->second.size() > 2){ // std::cout << "2+ range detected" << std::endl;
			auto b = std::next(seq_start);
			while(b != f->second.end()) if(m > b->first){ seq_start = b; b++; } else break;
			auto t = std::next(seq_end);
			while(t != f->second.rend()) if(m < t->first){ seq_end = t; t++; } else break;
		}

		double prog = (m - seq_start->first) / (seq_end->first - seq_start->first);

		if(m > seq_end->first) *(f->first) = seq_end->second; // went over the time limit
		else if(m < seq_start->first) *(f->first) = seq_start->second;
		else *(f->first) = seq_start->second + ((seq_end->second - seq_start->second) * prog);
		// else *(f->first) = r->second + ((m - r->first) * ((s->second - r->second) / (s->first - r->first)));
	}

	for(auto f = Topl_Timeline::double_map.begin(); f != double_map.end(); f++){
		std::map<millisec_t, double>::iterator seq_start = f->second.begin();
		std::map<millisec_t, double>::reverse_iterator seq_end = f->second.rbegin();

		if(f->second.size() > 2){ // std::cout << "2+ range detected" << std::endl;
			auto b = std::next(seq_start);
			while(b != f->second.end()) if(m > b->first){ seq_start = b; b++; } else break;
			auto t = std::next(seq_end);
			while(t != f->second.rend()) if(m < t->first){ seq_end = t; t++; } else break;
		}

		double prog = (m - seq_start->first) / (seq_end->first - seq_start->first);

		if(m > seq_end->first) *(f->first) = seq_end->second; // went over the time limit
		else if(m < seq_start->first) *(f->first) = seq_start->second;
		else *(f->first) = seq_start->second + ((seq_end->second - seq_start->second) * prog);
		// else *(f->first) = r->second + ((m - r->first) * ((s->second - r->second) / (s->first - r->first)));
	}
}

template<typename T>
void Topl_Timeline::addSequence(T var, std::pair<millisec_t, T> target){
    /* if(std::is_same(var, Vec3f)) logMessage("vec3f sequence detected");
	if(std::is_same(var, float)) logMessage("float sequence detected");
	if(std::is_same(var, double)) logMessage("double sequence detected");
    if(std::is_same(var, int)) logMessage("int sequence detected"); */
	// TODO: Implement body
}

void Topl_Timeline::addSequence_vec3f(Vec3f* var, std::pair<millisec_t, Vec3f> target){
	// addSequence(var, target);
	auto sequence = std::find_if(Topl_Timeline::vec3f_map.begin(), Topl_Timeline::vec3f_map.end(), [var](const std::pair<Vec3f*, std::map<millisec_t, Vec3f>>& p){ return p.first == var; });
	if(target.first == TIMELINE_AT) target.first = Topl_Program::timeline.dynamic_ticker.getAbsSecs();

	if(sequence != Topl_Timeline::vec3f_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::vec3f_map.insert({ var, std::map<millisec_t, Vec3f>() }); // create object
		if(target.first != 0.0) Topl_Timeline::vec3f_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::vec3f_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}

void Topl_Timeline::addSequence_float(float* var, std::pair<millisec_t, float> target){
	// addSequence(var, target);
	auto sequence = std::find_if(Topl_Timeline::float_map.begin(), Topl_Timeline::float_map.end(), [var](const std::pair<float*, std::map<millisec_t, float>>& p){ return p.first == var; });
	if(target.first == TIMELINE_AT) target.first = Topl_Program::timeline.dynamic_ticker.getAbsSecs();

	if(sequence != Topl_Timeline::float_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::float_map.insert({ var, std::map<millisec_t, float>() }); // create object
		if(target.first != 0.0) Topl_Timeline::float_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::float_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}

void Topl_Timeline::addSequence_double(double* var, std::pair<millisec_t, double> target){
	// addSequence(var, target);
	auto sequence = std::find_if(Topl_Timeline::double_map.begin(), Topl_Timeline::double_map.end(), [var](const std::pair<double*, std::map<millisec_t, double>>& p){ return p.first == var; });
	if(target.first == TIMELINE_AT) target.first = Topl_Program::timeline.dynamic_ticker.getAbsSecs();

	if(sequence != Topl_Timeline::double_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::double_map.insert({ var, std::map<millisec_t, double>() }); // create object
		if(target.first != 0.0) Topl_Timeline::double_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::double_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}
