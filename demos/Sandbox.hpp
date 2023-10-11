#include "Topl_Main.hpp"

struct Sandbox_Demo : public Topl_Main {
    Sandbox_Demo(const char* execPath, TARGET_Backend backend) : Topl_Main(execPath, "Sandbox", backend){}

    void init() override;
    void loop(double frameTime) override;
private:
    // TODO: Include geometry here
    // TODO: Include textures here
};