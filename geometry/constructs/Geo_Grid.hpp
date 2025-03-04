#ifndef GEO_GRID_H

#include "Geo_Construct.hpp"

// Grid Parameters

typedef std::pair<unsigned short, float> gridSize_pair;

struct Geo_Grid_Params {
	Geo_Grid_Params() {}
	Geo_Grid_Params(gridSize_pair size){ // Uniform Grid
		// assert(size.first() > 0);
    	x = size; y = size; z = size;
	}
	Geo_Grid_Params(gridSize_pair xSize, gridSize_pair ySize){ // Square Grid
		// assert(size.first() > 0);
		x = xSize; y = ySize;
	}
	Geo_Grid_Params(gridSize_pair xSize, gridSize_pair ySize, gridSize_pair zSize){ // Cube Grid
		// assert(xSize.first() > 0 && ySize.first() > 0 && zSize.first() > 0);
    	x = xSize; y = ySize; z = zSize;
	}

	unsigned getGridSize() const { return x.first * y.first * z.first; }

	/* double getProportion(AXIS_Target axis1, AXIS_Target axis2){ // TODO: Include distances?
		if(axis1 == axis2) return axis2;

		unsigned dividen = (axis1 == AXIS_X)? x.first : (axis1 == AXIS_Y)? y.first : z.first;
		switch(axis2){
			case AXIS_X: return (double)dividen / (double)x.first;
			case AXIS_Y: return (double)dividen / (double)y.first;
			case AXIS_Z: return (double)dividen / (double)z.first;
		}
	} */

	gridSize_pair x = std::make_pair(1, 0.0f); // x axis size and spacing
	gridSize_pair y = std::make_pair(1, 0.0f); // y axis size and spacing
	gridSize_pair z = std::make_pair(1, 0.0f); // z axis size and spacing
};

// Geometry Grid

class Geo_Grid : public Geo_Construct{
public:
	Geo_Grid(const std::string& prefix, const Geo_Mesh* mesh, const Geo_Grid_Params& params) 
	: Geo_Construct(prefix, mesh, params.getGridSize()){ // Non-configured Constuctor
        _params = params;
		_origin = Vec3f({
			-1.0f * params.x.first * (params.x.second * 0.5f) + (params.x.second / 2),
			-1.0f * params.y.first * (params.y.second * 0.5f) + (params.y.second / 2),
			DEFAULT_Z
		});
		init();
    }

    Geo_Grid(const std::string& prefix, Topl_Scene* scene, const Geo_Mesh* mesh, const Geo_Grid_Params& params) 
	: Geo_Construct(prefix, mesh, params.getGridSize()){ // Configured Constuctor
        _params = params;
		_origin = Vec3f({
			-1.0f * params.x.first * (params.x.second * 0.5f) + (params.x.second / 2),
			-1.0f * params.y.first * (params.y.second * 0.5f) + (params.y.second / 2),
			DEFAULT_Z
		});
		init();
		configure(scene);
    }
	
	std::string getCellName(unsigned num){ return getPrefix() + "cell" + std::to_string(num); }
	const Geo_Grid_Params* getParams(){ return &_params; }

    void init() override {
		Vec3f offset = Vec3f({ 0.0f, 0.0f, 0.0f });

		const unsigned short width = _params.x.first;
		const float x = _params.x.second;
		const unsigned short height = _params.y.first;
		const float y = _params.y.second;
		// const unsigned short depth = _params.z.first;
		const float z = _params.z.second;
		
		for(unsigned c = 0; c < _params.getGridSize(); c++){
			offset = Vec3f({
				(c % width) * x,
				((c % (height * width)) / width) * y,
				(c / (height * width)) * z
			});

			_geoActors[c].updatePos(_origin + offset);
		}
	}

	void configure(Topl_Scene* scene) override {
		for(unsigned c = 0; c < _params.getGridSize(); c++){
			scene->addGeometry(getCellName(c + 1), &_geoActors[c]);
#ifdef TOPL_ENABLE_PHYSICS
			scene->addPhysics(getCellName(c + 1), &_physActors.at(c));
#endif
		}
	}

	void scaleRow(unsigned short row, double amount){ for(unsigned c = 0; c < _params.getGridSize(); c++) std::cout << "Scaling rows with " << std::to_string(c) << ", "; }
	void scaleCol(unsigned short col, double amount){ for(unsigned c = 0; c < _params.getGridSize(); c++) std::cout << "Scaling cols with " << std::to_string(c) << ", "; }
	void scaleLen(unsigned short len, double amount){ for(unsigned c = 0; c < _params.getGridSize(); c++) std::cout << "Scaling lens with " << std::to_string(c) << ", "; }

protected:
    Geo_Grid_Params _params;
};

#define GEO_GRID_H
#endif
