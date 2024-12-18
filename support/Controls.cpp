#include "Controls.hpp"

#include <iostream>

// Keyboard Interaction

void Input_KeyControl::addKeyPress(char keyCode) {
	if (keyCode == '\0') return; // null keycode can be skipped
	for(std::vector<anyKeyCallback>::iterator m = _anyKeyCallbacks.begin(); m != _anyKeyCallbacks.end(); m++)
		(*m)(keyCode); // triggers callback
	for (std::map<char, keyCallback>::const_iterator c = _keyCallback_map.cbegin(); c != _keyCallback_map.cend(); c++)
		if(isalpha(keyCode)){ if (tolower(keyCode) == tolower(c->first)) c->second(); }// triggers callback if match
		else if(keyCode == c->first) c->second();
    for(auto function = _keyFunctions.begin(); function != _keyFunctions.end(); function++)
        (*function)(keyCode);
}

void Input_KeyControl::addCallback(char keyCode, keyCallback callback) {
	_keyCallback_map.insert(std::make_pair(keyCode, callback));
}

void Input_KeyControl::addAnyCallback(anyKeyCallback callback){
	_anyKeyCallbacks.push_back(callback);
}

void Input_KeyControl::addHandler(keyFunc function){ // New API
    _keyFunctions.push_back(function);
}

// Mouse Interaction

static Input_CursorRange defaultCursorRange = Input_CursorRange();

Input_CursorRange::Input_CursorRange(std::initializer_list<float> xRange, std::initializer_list<float> yRange){
	float x1 = *xRange.begin(); float y1 = *yRange.begin();
	float x2 = *(xRange.end() - 1); float y2 = *(yRange.end() - 1);

	xMin = (x1 < x2) ? x1 : x2; xMax = (x1 > x2) ? x1 : x2;
	yMin = (y1 < y2) ? y1 : y2; yMax = (y1 > y2) ? y1 : y2;
}

void Input_MouseControl::addCallback(enum MOUSE_Event event, pressCallback callback) {
	_pressCallback_map.insert(std::make_pair(event, callback));
}

#include <string>

void Input_MouseControl::addPress(enum MOUSE_Event event, float x, float y){
	if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press) _isMouseDown = std::make_pair(event, true);
	else if(event == MOUSE_LeftBtn_Release || event == MOUSE_RightBtn_Release) _isMouseDown = std::make_pair(event, false);
	
	for(std::map<MOUSE_Event, pressCallback>::const_iterator c = _pressCallback_map.cbegin(); c != _pressCallback_map.end(); c++)
		if(event == c->first) c->second(x, y); // makes callback go off where keys match

    for(auto function = _mouseFunctions.begin(); function != _mouseFunctions.end(); function++)
        (*function)(event, std::make_pair(x, y));
	
	// if(x != INVALID_CURSOR_POS && y != INVALID_CURSOR_POS && (event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press)){
	if(event == MOUSE_LeftBtn_Press || event == MOUSE_RightBtn_Press){
		_tracerSteps.push_back({ event, std::make_pair(x, y)}); // record the step
		_tracerPaths.push_back(Input_TracerPath(event)); // record the path
	}

	stampEvent();
}

void Input_MouseControl::addPress(enum MOUSE_Event event){
	addPress(event, INVALID_CURSOR_POS, INVALID_CURSOR_POS);
}

void Input_MouseControl::addHoverCallback(hoverCallback callback){
	_hoverCallback_map.insert(std::make_pair(&defaultCursorRange, callback));
}

void Input_MouseControl::addHover(float x, float y){
	for(std::map<const Input_CursorRange*, hoverCallback>::const_iterator c = _hoverCallback_map.cbegin(); c != _hoverCallback_map.end(); c++)
		if(x > c->first->xMin && x < c->first->xMax && y > c->first->yMin && y < c->first->yMax)
			c->second(x, y);
}

void Input_MouseControl::addDragCallback(hoverCallback callback){
	_dragCallback_map.insert(std::make_pair(&defaultCursorRange, callback));
}

void Input_MouseControl::addDrag(float x, float y){
	for(std::map<const Input_CursorRange*, dragCallback>::const_iterator c = _dragCallback_map.cbegin(); c != _dragCallback_map.end(); c++)
		if(x > c->first->xMin && x < c->first->xMax && y > c->first->yMin && y < c->first->yMax)
			c->second(x, y);

	if(x != INVALID_CURSOR_POS && y != INVALID_CURSOR_POS && !_tracerPaths.empty()){
		_tracerPaths.back().steps[_tracerPaths.back().stepsCount % MAX_PATH_STEPS] = { x, y };
		_tracerPaths.back().stepsCount++;
	}
}

void Input_MouseControl::addHandler(mouseFunc function){
    _mouseFunctions.push_back(function);
}

std::pair<float, float> Input_MouseControl::getLastStepDiff() const {
    unsigned short steps = _tracerSteps.size();
    if(steps >= 2)
        return std::make_pair(
            _tracerSteps[steps - 1].step.first - _tracerSteps[steps - 2].step.first,
            _tracerSteps[steps - 1].step.second - _tracerSteps[steps - 2].step.second
        );
    else return std::make_pair(0.0f, 0.0f);
}

std::pair<float, float> Input_MouseControl::getLastPathDiff() const {
    if(_tracerPaths.size() < 1) return std::make_pair(0.0f, 0.0f);

    Input_TracerPath lastPath = getTracerPaths()->back();
    if(lastPath.stepsCount >= 2)
        return std::make_pair(
            lastPath.steps[lastPath.stepsCount - 1].first - lastPath.steps[lastPath.stepsCount - 2].first,
            lastPath.steps[lastPath.stepsCount - 1].second - lastPath.steps[lastPath.stepsCount - 2].second
        );
    else return std::make_pair(0.0f, 0.0f);
}

