#include "Topl_Spacetime_Visualizer.hpp"

// Launches with DirectX 11 on Windows and OpenGL 4 elsewhere, pass --gl or --dx to override

MAIN_ENTRY {
	Topl_Spacetime_Visualizer* spacetime = new Topl_Spacetime_Visualizer(argv[0], Topl_Runtime::getBackendArg(argc, argv));
	spacetime->run();

	delete(spacetime);
	return 0;
}
