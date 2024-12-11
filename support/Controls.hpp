#ifndef CONTROLS_H

#include <map>
#include <algorithm>
#include <cctype>
#include <functional>

#include "Timer.hpp"

// Control

struct Input_Control {
    double getLastEvent(){ return _lastEvent; }
    double getEventCount(){ return _eventCount; }
protected:
    void stampEvent(){ // stamps that new event occured
        if(_lastEvent == INVALID_TIME) _ticker.reset();
        _lastEvent = _ticker.getRelMillisecs();
        _eventCount++;
    }

    Timer_Persist _ticker; // internal timer
    double _lastEvent = INVALID_TIME; // timestamp for most recent event
	unsigned _eventCount = 0;
};

// Keys

typedef void (*keyCallback)(void); // Triggers action on a specified keypress
typedef void (*anyKeyCallback)(char); // Triggers action on any keypress

typedef std::function<void(char)> keyFunc; // new type for key input

class Input_KeyControl : public Input_Control {
public:
	Input_KeyControl() : Input_Control(){}

	void addKeyPress(char keyCode);
	void addCallback(char keyCode, keyCallback callback);
    void addAnyCallback(anyKeyCallback callback);

    void addHandler(keyFunc function);
private:
    std::vector<anyKeyCallback> _anyKeyCallbacks;
	std::map<char, keyCallback> _keyCallback_map;

    std::vector<keyFunc> _keyFunctions;
};

// Mouse

#define INVALID_CURSOR_POS -10.0f // indicates that cursor is off the screen
#define MAX_CURSOR_BOUND 1.0f
#define MIN_CURSOR_BOUND -1.0f

enum MOUSE_Event {
    MOUSE_Hover,
    MOUSE_LeftBtn_Press,
    MOUSE_LeftBtn_Drag,
    MOUSE_LeftBtn_Release,
    MOUSE_RightBtn_Press,
    MOUSE_RightBtn_Drag,
    MOUSE_RightBtn_Release,
    MOUSE_MiddleBtn_Press,
    MOUSE_Scroll_Up,
    MOUSE_Scroll_Down
};

struct Input_TracerStep {
    MOUSE_Event button;
	std::pair<float, float> step;
};

#define MAX_PATH_STEPS 32

struct Input_TracerPath {
    Input_TracerPath(enum MOUSE_Event b){ button = b; } 

    // std::pair<float, float> getLastPath(unsigned short s){ return (s < stepsCount)? steps[stepsCount - s - 1] : steps[stepsCount - 1]; }

	MOUSE_Event button;
	std::pair<float, float> steps[MAX_PATH_STEPS];
	unsigned stepsCount = 0;
};

struct Input_CursorRange {
	Input_CursorRange(){} // maximum range constructor
	Input_CursorRange(std::initializer_list<float> xRange, std::initializer_list<float> yRange); // specified range constructor
    
    float xMin = MIN_CURSOR_BOUND; float xMax = MAX_CURSOR_BOUND;
    float yMin = MIN_CURSOR_BOUND; float yMax = MAX_CURSOR_BOUND;
};

typedef std::pair<unsigned, unsigned> tracerPath_t; // start and ending index of tracer steps inside of a path

typedef void (*mouseCallback)(enum MOUSE_Event, std::pair<float, float>); // Triggers action on any mouse event
typedef void (*pressCallback)(float, float); // Triggers action on a mouse button press
typedef void (*hoverCallback)(float, float); // Triggers action on a cursor hover over specified region
typedef void (*dragCallback)(float, float); // Triggers action on a cursor hover over specified region
typedef void (*scrollCallback)(bool); // Triggers action on wheel in positive or negative direction

typedef std::function<void(enum MOUSE_Event, std::pair<float, float>)> mouseFunc; // new type for mouse input

class Input_MouseControl : public Input_Control {
public:
    Input_MouseControl() : Input_Control(){}
    void setOnScroll(scrollCallback callback){ onScroll = callback; }
    void addCallback(enum MOUSE_Event mb, pressCallback callback);
    void addPress(enum MOUSE_Event mb, float x, float y);// mouse press with no cursor
    void addPress(enum MOUSE_Event mb); // mouse press no cursor
    void addHoverCallback(hoverCallback callback);
    void addHover(float x, float y); // checks for hover events given cursor position
    void addDragCallback(dragCallback callback);
    void addDrag(float x, float y);  // checks for drag events given cursor position
    
    void addHandler(mouseFunc function);

    scrollCallback onScroll = nullptr;
    std::pair<enum MOUSE_Event, bool> getIsMouseDown(){ return _isMouseDown; }

    const std::vector<Input_TracerStep>* getTracerSteps() const { return &_tracerSteps; }
    std::pair<float, float> getLastStepDiff() const;
    const std::vector<Input_TracerPath>* getTracerPaths() const { return &_tracerPaths; }
    std::pair<float, float> getLastPathDiff() const;
private:
    std::pair<enum MOUSE_Event, bool> _isMouseDown; // tracks state of mouse being held
    std::vector<Input_TracerStep> _tracerSteps; // tracks steps whenever mouse event and cursor pos is known 
    std::vector<Input_TracerPath> _tracerPaths; // tracks paths whenever mouse is held and moving

	std::vector<mouseCallback> _mouseCallbacks; // will store any action
    std::map<MOUSE_Event, pressCallback> _pressCallback_map;
    std::map<const Input_CursorRange*, hoverCallback> _hoverCallback_map;
    std::map<const Input_CursorRange*, dragCallback> _dragCallback_map;

    std::vector<mouseFunc> _mouseFunctions;
};

#define CONTROLS_H
#endif
