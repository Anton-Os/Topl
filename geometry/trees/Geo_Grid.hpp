#include "Geo_Tree.hpp"

// Parameters

typedef std::pair<unsigned short, float> gridAttrib_pair;

struct Geo_Grid_Params {
	Geo_Grid_Params() {}
	Geo_Grid_Params(gridAttrib_pair xyzAttrib); // Uniform sides constructor
	Geo_Grid_Params( // Variable sides constructor
		gridAttrib_pair xAttrib,
		gridAttrib_pair yAttrib,
		gridAttrib_pair zAttrib
	);

	void clamp(); // ensures units are valid values
	unsigned getCellCount() const { return x.first * y.first * z.first; }

	gridAttrib_pair x = std::make_pair(1, 0.0f); // x axis count and spacing
	gridAttrib_pair y = std::make_pair(1, 0.0f); // y axis count and spacing
	gridAttrib_pair z = std::make_pair(1, 0.0f); // z axis count and spacing
};

// Geometry Grid

class Geo_Grid : public Geo_Tree, public Geo_DynamicSet {
public:
	Geo_Grid( // Prebake Constructor
        const std::string& prefix,
        const Geo_Actor* geo, 
        const Geo_Grid_Params& params
	) : Geo_Tree(prefix, geo, params.getCellCount()),
	Geo_DynamicSet(params.getCellCount()){
        _params = params;
		_origin = Vec3f({
			-1.0f * params.x.first * (params.x.second * 0.5f) + (params.x.second / 2),
			-1.0f * params.y.first * (params.y.second * 0.5f) + (params.y.second / 2),
			0.0f
		});
    }

    Geo_Grid( // Config Constructor
        const std::string& prefix, 
        Topl_Scene* scene, 
        const Geo_Actor* geo, 
        const Geo_Grid_Params& params
	) : Geo_Tree(prefix, geo, params.getCellCount()),
	Geo_DynamicSet(params.getCellCount()){
        _params = params;
		_origin = Vec3f({
			-1.0f * params.x.first * (params.x.second * 0.5f) + (params.x.second / 2),
			-1.0f * params.y.first * (params.y.second * 0.5f) + (params.y.second / 2),
			0.0f
		});
		configure(scene);
    }

	void configure(Topl_Scene* scene) override;
private:
	Vec3f _origin;
    Geo_Grid_Params _params;
};
