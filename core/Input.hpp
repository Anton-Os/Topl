// #include <ctime>
#include <queue>
#include <vector>

#include "Timer.hpp"

enum KEY_Event {
    KEY_none,
    KEY_press,
    KEY_dbl_press,
    KEY_release
};

struct KeyState {
    // KeyState(){}
    // KeyState(const char* kc, enum KEY_Event ke, double ts, double te){
    KeyState(const char* kc, enum KEY_Event ke, double lt){
        keyCode = kc; event = ke; lastStrokeT = lt;
    }
    const char* keyCode;
    // int keyID;
    KEY_Event event;

    double lastStrokeT; // Stores the time of last keystroke
    // Timestamp timestamp; // Try to separate into seconds and milliseconds
    // Timestamp expireTime; // Same here, = timestamp + keyLogger::expire
};

typedef void (*keyCallback)(unsigned, KeyState**);

class Input_KeyLogger {
public: 
    // Returns number of "expired" Keystates
    unsigned short updateKeyStates();

    void addKeyEvent(const char* keyCode, enum KEY_Event event);
private:
    std::queue<KeyState> mStates;
    std::vector<keyCallback> mCallbacks;

    Timer_Interval mTimer;
    double mExpireMil = 2.0; // How long do key events stay in the queue?
};