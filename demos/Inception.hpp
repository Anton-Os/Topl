#include "Topl_App.hpp"

struct Inception_App : public Topl_App {
    Inception_App(const char* execPath, APP_Backend b) 
		: Topl_App(execPath, "Inception", b){}

    void init() override {
        return;
    }
    void loop() override {
        return;
    }
};