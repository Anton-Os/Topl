#include "Topl_Visualizer.hpp"

// Launches with DirectX 11 on Windows and OpenGL 4 elsewhere, pass --gl or --dx to override

MAIN_ENTRY {
	Topl_Visualizer* visualizer = new Topl_Visualizer(argv[0], Topl_Runtime::getBackendArg(argc, argv));
	visualizer->run();

	delete(visualizer);
	return 0;
}
