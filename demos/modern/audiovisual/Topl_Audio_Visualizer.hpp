#include "../Topl_Runtime.hpp"

class Topl_Audio_Visualizer : public Topl_Runtime {
public:
    Topl_Audio_Visualizer(BACKEND_Target backend = BACKEND_GL4) : Topl_Runtime(backend) { }
};