#include "Topl_Ragdolls.hpp"

// Launches with DirectX 11 on Windows and OpenGL 4 elsewhere, pass --gl or --dx to override

MAIN_ENTRY {
	Topl_Ragdolls* demo = new Topl_Ragdolls(argv[0], Topl_Runtime::getBackendArg(argc, argv));
	demo->run();

	delete(demo);
	return 0;
}
