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
	for(std::map<MOUSE_Button, keyCallback>::const_iterator currentCallback = _mouseCallback_map.cbegin(); currentCallback != _mouseCallback_map.end(); currentCallback++)
		if(mb == currentCallback->first) currentCallback->second(); // Makes callback go off
}

void Input_MouseLogger::addCallback(enum MOUSE_Button mb, keyCallback callback) {
	_mouseCallback_map.insert(std::make_pair(mb, callback));
}