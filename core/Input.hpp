
#include <vector>
#include <map>

#include "Timer.hpp"

enum KEY_Event {
    KEY_none = 0,
    KEY_press,
    KEY_dbl_press,
    KEY_release
};

struct KeyState {
    // KeyState(){}
    // KeyState(const char* kc, enum KEY_Event ke, double ts, double te){
    KeyState(){}
    KeyState(char kc, enum KEY_Event ke){
        keyCode = kc; event = ke;
    }
    KeyState(char kc, enum KEY_Event ke, double lt){
        keyCode = kc; event = ke; tstampMil = lt;
    }
    char keyCode = '\0';
    // int keyID;
    KEY_Event event = KEY_none;

	double tstampMil = 0.0f; // Timestamp in milliseconds when event triggered
	double holdTime = 0.0f; // Duration the key was held
};


typedef void (*keyComboCallback)(const std::vector<KeyState>& states); // Accept number of keys and keystates
typedef void (*keyCallback)(void); // Simply trigger on a certain

class Input_KeyLogger {
public: 
    // Returns number of "expired" Keystates
    // unsigned short updateKeyStates();

	KeyState getKeyState(char c);
    unsigned short getCallbackCount(){ return mCallbacks.size(); }

    void addKeyEvent(char keyCode, enum KEY_Event event); // Searches for callback and triggers it

	void addCallback(const KeyState* state, keyCallback callback);
    void addComboCallback(const std::vector<KeyState>& states, keyComboCallback callback); // States and the callback are closely tied 
    // void addCallback(keyCallback callback, std::vector<KeyState> states);
private:
    // std::queue<KeyState> mStates;
    std::vector<keyComboCallback> mComboCallbacks; // Populates vectors below

    std::vector<keyCallback> mCallbacks;
	std::vector<KeyState> mStates;
    std::vector<const KeyState*> mTriggerStates; // Fed in by the add callback function
	std::map<const char*, KeyState*> mCodeToKey_map; // Associates a character with a keyState, should be modifiable
    std::map<const char*, const KeyState*> mCodeToTrigger_map; // Associates a character with a keyTrigger
    //std::vector<char> mSupportKeys;

    Timer_Interval mTimer;
    //double mExpireMil = 2.0; // How long do key events stay in the queue?
};