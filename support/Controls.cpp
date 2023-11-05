#include "Controls.hpp"

// Keyboard Interaction

void Input_KeyControl::addKeyPress(char keyCode) {
	if (keyCode == '\0') return; // null keycode can be skipped
	for (std::map<char, keyCallback>::const_iterator c = _keyCallback_map.cbegin(); c != _keyCallback_map.cend(); c++)
		if (keyCode == c->first) c->second(); // Makes callback go off
	
	stampEvent();
}

void Input_KeyControl::addCallback(char keyCode, keyCallback callback) {
	_keyCallback_map.insert(std::make_pair(keyCode, callback));
}

// Mouse Interaction

Input_CursorRange::Input_CursorRange(std::initializer_list<float> xRange, std::initializer_list<float> yRange){
	float x1 = *xRange.begin(); float y1 = *yRange.begin();
	float x2 = *(xRange.end() - 1); float y2 = *(yRange.end() - 1);

	xMin = (x1 < x2) ? x1 : x2; 
	xMax = (x1 > x2) ? x1 : x2;
	yMin = (y1 < y2) ? y1 : y2; 
	yMax = (y1 > y2) ? y1 : y2;
}

void Input_MouseControl::addCallback(enum MOUSE_Button mb, pressCallback callback) {
	_mouseCallback_map.insert(std::make_pair(mb, callback));
}

void Input_MouseControl::addMousePress(enum MOUSE_Button mb){
	for(std::map<MOUSE_Button, pressCallback>::const_iterator c = _mouseCallback_map.cbegin(); c != _mouseCallback_map.end(); c++)
		if(mb == c->first) c->second(INVALID_CURSOR_POS, INVALID_CURSOR_POS); // makes callback go off where keys match

	stampEvent();
}

void Input_MouseControl::addMousePress(enum MOUSE_Button mb, float x, float y){
	for(std::map<MOUSE_Button, pressCallback>::const_iterator c = _mouseCallback_map.cbegin(); c != _mouseCallback_map.end(); c++)
		if(mb == c->first) c->second(x, y); // makes callback go off where keys match
	
	_tracerSteps.push_back({ mb, std::make_pair(x, y)}); // record the movement
	stampEvent();
}

void Input_MouseControl::addHoverCallback(const Input_CursorRange* range, hoverCallback callback) {
	_hoverCallback_map.insert(std::make_pair(range, callback));
}

void Input_MouseControl::addHover(float x, float y){
	for(std::map<const Input_CursorRange*, hoverCallback>::const_iterator c = _hoverCallback_map.cbegin(); c != _hoverCallback_map.end(); c++)
		if(x > c->first->xMin && x < c->first->xMax && y > c->first->yMin && y < c->first->yMax)
			c->second(x, y);
}