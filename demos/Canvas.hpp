#include "Platform.hpp"
#include "Topl_Renderer.hpp"
#include "Topl_Scene.hpp"

#include "primitives/Geo_Flat.hpp"

namespace Topl {
	// Management Objects
	Topl_Scene scene;

    Geo_FlatSquare canvas = Geo_FlatSquare(2.0f);
    Geo_Component canvasGeo  = Geo_Component((Geo_RenderObj*)&canvas);
}

/* void init(Platform* platform);
void gameLoop(const Topl_Renderer* renderer, Topl_Scene* scene); */