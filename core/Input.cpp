#include "Input.hpp"

#include <cstdio>

KeyState Input_KeyLogger::getKeyState(char c) {
	KeyState temp('\0', KEY_none, 0.0); // Improve this
	return temp;
}

void Input_KeyLogger::addKeyEvent(char keyCode, enum KEY_Event event){
    if(mCodeToKey_map.find(keyCode) == mCodeToKey_map.end()) // No callback associated
	    return;
    
	// Update current key status // Update hold time-
    KeyState* currentState = mCodeToKey_map.at(keyCode);
	currentState->keyCode = keyCode; // TYODO: Fix this, should be initialized already
    currentState->event = event;
    currentState->tstampMil = mTicker.getAbsMillsecs();

    const KeyState* triggerState = mCodeToTrigger_map.at(keyCode);

	bool statesMatch = 
        (triggerState->event == currentState->event && 
        (char)toupper(triggerState->keyCode) == (char)toupper(currentState->keyCode))
		? true : false; // Test for keycode and event matchup

    if(statesMatch) 
		mTriggerToCallback_map.at(triggerState)(); // Executes associated callback
}

void Input_KeyLogger::addCallback(const KeyState* state, keyCallback callback) {

    KeyState kState(state->keyCode, KEY_none);

    mStates.push_back(kState); // Adds the current state
    mCodeToKey_map.insert(  { state->keyCode, &mStates.at(mStates.size() - 1) }  );

    mTriggerStates.push_back(state); // Adds the trigger state
    mCodeToTrigger_map.insert(  { state->keyCode, mTriggerStates.at(mTriggerStates.size() - 1) }  );

    mCallbacks.push_back(callback); // Adds a callback
    mTriggerToCallback_map.insert(  { mTriggerStates.at(mTriggerStates.size() - 1), mCallbacks.at(mCallbacks.size() - 1)} );

	return;
}