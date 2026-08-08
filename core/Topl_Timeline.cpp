#include "Topl_Timeline.hpp"

std::map<Vec3f*, std::map<millisec_t, Vec3f>> Topl_Timeline::vec3f_map = std::map<Vec3f*, std::map<millisec_t, Vec3f>>();
std::map<float*, std::map<millisec_t, float>> Topl_Timeline::float_map = std::map<float*, std::map<millisec_t, float>>();
std::map<double*, std::map<millisec_t, double>> Topl_Timeline::double_map = std::map<double*, std::map<millisec_t, double>>();

template<typename T> 
void Topl_Timeline::updateSequence(T map, millisec_t time){
	for (auto& entry : storage) {
        T* var = entry.first;
        auto& seq = entry.second;

        auto seq_start = seq.begin();
        auto seq_end   = seq.rbegin();

        if (seq.size() > 2) {
            auto b = std::next(seq_start);
            while (b != seq.end() && m > b->first) { seq_start = b; ++b; }

            auto t = std::next(seq_end);
            while (t != seq.rend() && m < t->first) { seq_end = t; ++t; }
        }

        double prog = (m - seq_start->first) / (seq_end->first - seq_start->first);

        if (m > seq_end->first) *var = seq_end->second;
        else if (m < seq_start->first) *var = seq_start->second;
        else *var = seq_start->second + ((seq_end->second - seq_start->second) * prog);
    }
}

template<typename T> void Topl_Timeline::addSequence(T* var, std::pair<millisec_t, T> target){
	auto& storage = TimelineStorage<T>::map;

    // Fix time semantics
    if (target.first == TIMELINE_AT)
        target.first = dynamic_ticker.getAbsSecs();
    else if (target.first < 0.0F)
        target.first = dynamic_ticker.getAbsSecs() + std::abs(target.first);

    auto it = storage.find(var);

    if (it != storage.end()) {
        it->second.insert({ target.first, target.second });
    } else {
        storage[var] = std::map<millisec_t, T>();
        storage[var].insert({ TIMELINE_START, *var });
        storage[var].insert({ target.first, target.second });
    }
}

template<typename T> void Topl_Timeline::addPeriodic(T* var, std::pair<millisec_t, T> target, unsigned short reps){
	for (unsigned r = 0; r < reps; r++) {
        if (r > 0) addSequence(var, { target.first * (r * 2), *var });
        addSequence(var, { target.first * ((r * 2) + 1), target.second });
    }
}

void Topl_Timeline::seqCallback(double m){
	// updateSequence(TimelineStorage<Vec3f>::map, m);
    // updateSequence(TimelineStorage<float>::map, m);
    // updateSequence(TimelineStorage<double>::map, m);

	// TODO: Remove all the code below for unecessary duplication

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

void Topl_Timeline::addSequence(Vec3f* var, std::pair<millisec_t, Vec3f> target){
	// addSequence(var, target);
	auto sequence = std::find_if(Topl_Timeline::vec3f_map.begin(), Topl_Timeline::vec3f_map.end(), [var](const std::pair<Vec3f*, std::map<millisec_t, Vec3f>>& p){ return p.first == var; });
	if(target.first == TIMELINE_AT) target.first = dynamic_ticker.getAbsSecs(); // current time
	else if(target.first < 0.0F) target.first = dynamic_ticker.getAbsSecs() + fabs(target.first); // future time

	if(sequence != Topl_Timeline::vec3f_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::vec3f_map.insert({ var, std::map<millisec_t, Vec3f>() }); // create object
		if(target.first != 0.0) Topl_Timeline::vec3f_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::vec3f_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}

void Topl_Timeline::addPeriodic(Vec3f* var, std::pair<millisec_t, Vec3f> target, unsigned short reps){
	for(unsigned r = 0; r < reps; r++){
		if(r > 0) addSequence(var, std::make_pair(target.first * (r * 2), *var)); // sets to return to current value
		addSequence(var, std::make_pair(target.first * ((r * 2) + 1), target.second)); // sets to return to new value
	}
}

void Topl_Timeline::addSequence(float* var, std::pair<millisec_t, float> target){
	// addSequence(var, target);
	auto sequence = std::find_if(Topl_Timeline::float_map.begin(), Topl_Timeline::float_map.end(), [var](const std::pair<float*, std::map<millisec_t, float>>& p){ return p.first == var; });
	if(target.first == TIMELINE_AT) target.first = dynamic_ticker.getAbsSecs();

	if(sequence != Topl_Timeline::float_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::float_map.insert({ var, std::map<millisec_t, float>() }); // create object
		if(target.first != 0.0) Topl_Timeline::float_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::float_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}

void Topl_Timeline::addPeriodic(float* var, std::pair<millisec_t, float> target, unsigned short reps){
	for(unsigned r = 0; r < reps; r++){
		if(r > 0) addSequence(var, std::make_pair(target.first * (r * 2), *var)); // sets to return to current value
		addSequence(var, std::make_pair(target.first * ((r * 2) + 1), target.second)); // sets to return to new value
	}
}

void Topl_Timeline::addSequence(double* var, std::pair<millisec_t, double> target){
	// addSequence(var, target);
	auto sequence = std::find_if(Topl_Timeline::double_map.begin(), Topl_Timeline::double_map.end(), [var](const std::pair<double*, std::map<millisec_t, double>>& p){ return p.first == var; });
	if(target.first == TIMELINE_AT) target.first = dynamic_ticker.getAbsSecs();

	if(sequence != Topl_Timeline::double_map.end()) sequence->second.insert({ target.first, target.second });
	else {
		Topl_Timeline::double_map.insert({ var, std::map<millisec_t, double>() }); // create object
		if(target.first != 0.0) Topl_Timeline::double_map[var].insert({ TIMELINE_START, *var }); // create default state at timeline start
		Topl_Timeline::double_map[var].insert({ target.first, target.second }); // insert updated state at new time
	}
}
