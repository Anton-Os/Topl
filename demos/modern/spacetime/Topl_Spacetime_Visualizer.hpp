#include "../Topl_Runtime.hpp"

class Topl_Spacetime_Visualizer : public Topl_Runtime {
public:
    Topl_Spacetime_Visualizer(BACKEND_Target backend = BACKEND_GL4) : Topl_Runtime(backend) { }
};