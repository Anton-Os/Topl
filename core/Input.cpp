#include "Input.hpp"

#include <cstdio>

void Input_KeyLogger::addKeyPress(char keyCode) {
	if (keyCode == '\0') return; // null keycode can be skippedS
	for (std::map<char, keyCallback>::const_iterator currentCallback = _keyCallback_map.cbegin(); currentCallback != _keyCallback_map.cend(); currentCallback++)
		if (keyCode == currentCallback->first) currentCallback->second(); // Makes callback go off
}

void Input_KeyLogger::addCallback(char keyCode, keyCallback callback) {
	_keyCallback_map.insert(std::make_pair(keyCode, callback));
}

void Input_MouseLogger::addMousePress(enum MOUSE_Button mb){
	for(std::map<MOUSE_Button, pressCallback>::const_iterator currentCallback = _mouseCallback_map.cbegin(); currentCallback != _mouseCallback_map.end(); currentCallback++)
		if(mb == currentCallback->first) currentCallback->second(); // Makes callback go off
}

void Input_MouseLogger::addMousePress(enum MOUSE_Button mb, float x, float y){
	for(std::map<MOUSE_Button, pressCallback>::const_iterator currentCallback = _mouseCallback_map.cbegin(); currentCallback != _mouseCallback_map.end(); currentCallback++)
		if(mb == currentCallback->first) currentCallback->second(); // Makes callback go off
	
	_tracerSteps.push_back({ mb, x, y}); // record the movement
}

void Input_MouseLogger::addCallback(enum MOUSE_Button mb, pressCallback callback) {
	_mouseCallback_map.insert(std::make_pair(mb, callback));
}

void Input_MouseLogger::addHoverCallback(const Input_CursorRange* range, hoverCallback callback) {
	_hoverCallback_map.insert(std::make_pair(range, callback));
}