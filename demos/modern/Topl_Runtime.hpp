#include "Topl_Factory.hpp"

class Topl_Runtime  {
public:
    Topl_Runtime(BACKEND_Target backend = BACKEND_GL4) : _backend(backend) { }
private:
    BACKEND_Target _backend;
};