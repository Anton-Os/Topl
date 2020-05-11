#include "Input.hpp"

/* void Input_KeyLogger::addKeyEvent(const char* keyCode, enum KeyState::KEY_Event event){
    KeyState state; // Replace the following code with a constructor invocation
    state.mKeyCode = keyCode;
    state.mEvent = event;
    state.mTimestamp = mTimer.
} */

/*
void Input_KeyLogger::addKeyEvent(const char* keyCode, enum KEY_Event event){
    KeyState kState(keyCode, event, mTimer.getSecsPassed());
    mStates.push(kState);

    updateKeyStates();
}

unsigned short Input_KeyLogger::updateKeyStates(){
    double strokeTimeBuff = 0.0f;
    unsigned short expiredKeys = 0;

    // Make sure to check for "prolonged" key presses in next implementation
    // A keypress has to be followed by a release for mStates to pop
    for(unsigned k = 0; k < mStates.size(); k++){
        if(strokeTimeBuff >= mExpireMil){
            mStates.pop(); // Keystroke time expired!
            expiredKeys++;
        }
        else
            strokeTimeBuff += mStates.back().lastStrokeT;
    }

    return expiredKeys;
} */

KeyState Input_KeyLogger::getKeyState(char c) {
	KeyState temp('\0', KEY_none, 0.0);
	return temp;
}

void Input_KeyLogger::addKeyEvent(const char* keyCode, enum KEY_Event event){
	return;
}

void Input_KeyLogger::addCallback(const KeyState* state, keyCallback callback) {
	return;
}