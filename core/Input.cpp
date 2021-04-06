#include "Input.hpp"

#include <cstdio>

void Input_KeyLogger::addKeyPress(char keyCode) {
	for (std::map<char, keyCallback>::const_iterator currentCallback = mKeyCallback_map.cbegin(); currentCallback != mKeyCallback_map.cend(); currentCallback++)
		if (keyCode == currentCallback->first) currentCallback->second(); // Makes callback go off
}

void Input_KeyLogger::addCallback(char keyCode, keyCallback callback) {
	mKeyCallback_map.insert(std::make_pair(keyCode, callback));
}

void Input_MouseLogger::addMousePress(enum MOUSE_Button mb){
	for(std::map<MOUSE_Button, keyCallback>::const_iterator currentCallback = mMouseCallback_map.cbegin(); currentCallback != mMouseCallback_map.end(); currentCallback++)
		if(mb == currentCallback->first) currentCallback->second(); // Makes callback go off
}

void Input_MouseLogger::addCallback(enum MOUSE_Button mb, keyCallback callback) {
	mMouseCallback_map.insert(std::make_pair(mb, callback));
}