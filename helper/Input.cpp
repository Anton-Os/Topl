#include "Input.hpp"

#include <cstdio>

// Keyboard Interaction

void Input_KeyLogger::addKeyPress(char keyCode) {
	if (keyCode == '\0') return; // null keycode can be skippedS
	for (std::map<char, keyCallback>::const_iterator c = _keyCallback_map.cbegin(); c != _keyCallback_map.cend(); c++)
		if (keyCode == c->first) c->second(); // Makes callback go off
}

void Input_KeyLogger::addCallback(char keyCode, keyCallback callback) {
	_keyCallback_map.insert(std::make_pair(keyCode, callback));
}

// Mouse Interaction

void Input_MouseLogger::addCallback(enum MOUSE_Button mb, pressCallback callback) {
	_mouseCallback_map.insert(std::make_pair(mb, callback));
}

void Input_MouseLogger::addMousePress(enum MOUSE_Button mb){
	for(std::map<MOUSE_Button, pressCallback>::const_iterator c = _mouseCallback_map.cbegin(); c != _mouseCallback_map.end(); c++)
		if(mb == c->first) c->second(BAD_CURSOR_POS, BAD_CURSOR_POS); // makes callback go off where keys match
}

void Input_MouseLogger::addMousePress(enum MOUSE_Button mb, float x, float y){
	for(std::map<MOUSE_Button, pressCallback>::const_iterator c = _mouseCallback_map.cbegin(); c != _mouseCallback_map.end(); c++)
		if(mb == c->first) c->second(x, y); // makes callback go off where keys match
	
	_tracerSteps.push_back({ mb, x, y}); // record the movement
}

void Input_MouseLogger::addHoverCallback(const Input_CursorRange* range, hoverCallback callback) {
	_hoverCallback_map.insert(std::make_pair(range, callback));
}

void Input_MouseLogger::addHover(float x, float y){
	for(std::map<const Input_CursorRange*, hoverCallback>::const_iterator c = _hoverCallback_map.cbegin(); c != _hoverCallback_map.end(); c++)
		if(x > c->first->xMin && x < c->first->xMax && y > c->first->yMin && y < c->first->yMax)
			c->second(x, y);
}