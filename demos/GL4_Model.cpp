#include "GL4_Model.hpp"

int main(int argc, char** argv) {

	Platform platform(argv[0]);
	platform.createWindow("Model");

	Topl_Renderer_GL4 renderer(platform.getNativeWindow());
	/* for (unsigned m = 0; m < MAX_MODEL_COUNT; m++) {
		Topl::model = Geo_Model3D(Topl::docTree.getNode(m));
		// Topl::models.push_back(model);
		Topl::geos.push_back((Geo_RenderObj*)&Topl::model);
	} */
	// for(unsigned g = 0; g < MAX_MODEL_COUNT; g++) Topl::scene.addGeometry(Topl::docTree.getNodeName(g), &Topl::geos[g]);
	Topl::scene.addGeometry(Topl::docTree.getNodeName(0), &Topl::geo); // gets the first node name

	std::string vertexShaderSrc = getParentDir(argv[0]) + "\\Vertex_Volumes.glsl";
	VertexShader vertexShader = VertexShader(vertexShaderSrc.c_str());
	std::string fragmentShaderSrc = getParentDir(argv[0]) + "\\Frag_Flat.glsl";
	FragmentShader fragmentShader = FragmentShader(fragmentShaderSrc.c_str());

	renderer.setPipeline(&vertexShader, &fragmentShader);
	
	renderer.buildScene(&Topl::scene);
	Topl::scene.setCamera(false, SpatialBounds3D(30.0f));

	glPointSize(5.0f); // TODO: Include as part of the renderer code
	glLineWidth(3.0f); // TODO: Include as part of the renderer code
	// Topl::geo.updatePos(Eigen::Vector3f(0.0f, -40.0f, 0.0f));

	Timer_Ticker gameTicker;
	while (renderer.renderScene(DRAW_Points)) {
		Topl::geo.updateRot(Eigen::Vector2f(0.0, 0.001 * gameTicker.getAbsSecs()));
		// Topl::geo.updateRot(Eigen::Vector2f(0.001 * gameTicker.getAbsSecs(), 0.0f));

		renderer.updateScene(&Topl::scene);
		platform.handleEvents();
	}

	return 0;
}