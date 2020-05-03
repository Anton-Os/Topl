// #include <ctime>
#include <queue>
#include <vector>

#include "Timer.hpp"

enum KEY_Event {
    KEY_press,
    KEY_dbl_press,
    KEY_release
};

struct KeyState {
    // KeyState(){}
    // KeyState(const char* kc, enum KEY_Event ke, double ts, double te){
    KeyState(const char* kc, enum KEY_Event ke, Timestamp ts, Timestamp tse){
        keyCode = kc; event = ke; timestamp = ts; expireTime = tse;
    }
    const char* keyCode;
    // int keyID;
    KEY_Event event;

    Timestamp timestamp; // Try to separate into seconds and milliseconds
    Timestamp expireTime; // Same here, = timestamp + keyLogger::expire
};

typedef void (*keyCallback)(unsigned, KeyState**);

class Input_KeyLogger {
public: 
    // Returns number of "expired" Keystates
    unsigned short updateKeyStates();

    // void addKeyEvent(const char* keyCode, enum KeyState::KEY_Event event);
private:
    std::queue<KeyState> mStates;
    std::vector<keyCallback> mCallbacks;

    Topl_Timer mTimer;
    double mExpireMil = 1.0; // How long do key events stay in the queue?
};