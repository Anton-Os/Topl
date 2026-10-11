#include "../Topl_Runtime.hpp"

class Topl_Visualizer : public Topl_Runtime {
public:
    Topl_Visualizer(BACKEND_Target backend = BACKEND_GL4) : Topl_Runtime(backend) { }
};