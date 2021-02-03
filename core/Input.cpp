#include "Input.hpp"

#include <cstdio>

void Input_KeyLogger::addKeyPress(char keyCode) {
	for (std::map<char, keyCallback>::const_iterator currentCallback = mKeyCallback_map.cbegin(); currentCallback != mKeyCallback_map.cend(); currentCallback++)
		if (keyCode == currentCallback->first) currentCallback->second(); // Makes callback go off
}

void Input_KeyLogger::addCallback(char keyCode, keyCallback callback) {
	mKeyCallback_map.insert(std::make_pair(keyCode, callback));
}