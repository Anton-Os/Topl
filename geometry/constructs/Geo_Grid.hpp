#ifndef GEO_GRID_H

#include "Geo_Construct.hpp"

// Grid Parameters

typedef std::pair<unsigned short, float> gridSize_pair;

struct Geo_Grid_Params {
	Geo_Grid_Params() {}
	Geo_Grid_Params(gridSize_pair size){
		// assert(size > 0);
    	x = size;  y = size; z = size;
	}
	Geo_Grid_Params(gridSize_pair xSize, gridSize_pair ySize, gridSize_pair zSize){
		// assert(xSize > 0 && ySize > 0 && zSize > 0);
    	x = xSize; y = ySize; z = zSize;
	}

	unsigned getGridSize() const { return x.first * y.first * z.first; }

	gridSize_pair x = std::make_pair(1, 0.0f); // x axis size and spacing
	gridSize_pair y = std::make_pair(1, 0.0f); // y axis size and spacing
	gridSize_pair z = std::make_pair(1, 0.0f); // z axis size and spacing
};

// Geometry Grid

class Geo_Grid : public Geo_Construct{
public:
	Geo_Grid(const std::string& prefix, const Geo_Actor* actor, const Geo_Grid_Params& params) 
	: Geo_Construct(prefix, actor, params.getGridSize()){ // Non-configured Constuctor
        _params = params;
		_origin = Vec3f({
			-1.0f * params.x.first * (params.x.second * 0.5f) + (params.x.second / 2),
			-1.0f * params.y.first * (params.y.second * 0.5f) + (params.y.second / 2),
			0.0f
		});
    }

    Geo_Grid(const std::string& prefix, Topl_Scene* scene, const Geo_Actor* actor, const Geo_Grid_Params& params) 
	: Geo_Construct(prefix, actor, params.getGridSize()){ // Configured Constuctor
        _params = params;
		_origin = Vec3f({
			-1.0f * params.x.first * (params.x.second * 0.5f) + (params.x.second / 2),
			-1.0f * params.y.first * (params.y.second * 0.5f) + (params.y.second / 2),
			0.0f
		});
		configure(scene);
    }
	
	static std::string genCellName(unsigned num){ return "cell" + std::to_string(num); }

	void configure(Topl_Scene* scene) override {
		Vec3f offset = Vec3f({ 0.0f, 0.0f, 0.0f });

		const unsigned short width = _params.x.first;
		const float x = _params.x.second;
		const unsigned short height = _params.y.first;
		const float y = _params.y.second;
		const unsigned short depth = _params.z.first;
		const float z = _params.z.second;
		
		for(unsigned c = 0; c < _geoActors.size(); c++){
			offset = Vec3f({
				((c % width) * x),
				((c % (height * width)) / width) * y,
				(c / (height * width)) * z
			});

			_geoActors[c].updatePos(_origin + offset);
			scene->addGeometry(getPrefix() + genCellName(c + 1), &_geoActors[c]);
			// scene->addPhysics(getPrefix() + genCellName(c + 1), &_physActors.at(c));
		}
	}
private:
    Geo_Grid_Params _params;
};

#define GEO_GRID_H
#endif