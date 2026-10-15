#include "Topl_Audio_Visualizer.hpp"

// Launches with DirectX 11 on Windows and OpenGL 4 elsewhere, pass --gl or --dx to override

MAIN_ENTRY {
	Topl_Audio_Visualizer* audiovisual = new Topl_Audio_Visualizer(argv[0], Topl_Runtime::getBackendArg(argc, argv));
	audiovisual->run();

	delete(audiovisual);
	return 0;
}
