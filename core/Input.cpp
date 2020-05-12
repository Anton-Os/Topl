#include "Input.hpp"

#include <cstdio>

KeyState Input_KeyLogger::getKeyState(char c) {
	KeyState temp('\0', KEY_none, 0.0); // Improve this
	return temp;
}

void Input_KeyLogger::addKeyEvent(char keyCode, enum KEY_Event event){
    if(mCodeToKey_map.find(&keyCode) == mCodeToKey_map.end()) // No callback associated
	    return;
    
    KeyState* currentState = mCodeToKey_map.at(&keyCode);
    currentState->event = event;

    // Make this better
    currentState->tstampMil = mTimer.getAbsMillsecs();

    // For now just compare the event types
    const KeyState* triggerState = mCodeToTrigger_map.at(&keyCode);

    if(triggerState->event == currentState->event)
        puts("Event Match!");
}

void Input_KeyLogger::addCallback(const KeyState* state, keyCallback callback) {

    mTriggerStates.push_back(state); // Adds the trigger state
    KeyState kState(state->keyCode, KEY_none);
    mStates.push_back(kState); // Adds the current state

    mCodeToKey_map.insert(  {&state->keyCode, &mStates.at(mStates.size() - 1)}  );
    mCodeToTrigger_map.insert(  {&state->keyCode, mTriggerStates.at(mTriggerStates.size() - 1)}  );

    mCallbacks.push_back(callback);
	return;
}