#include <map>
#include <algorithm>
#include <cctype>

#include "Timer.hpp";

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

class Input_KeyControl : public Input_Control {
public:
	Input_KeyControl() : Input_Control(){}
	void addKeyPress(char keyCode);
	void addCallback(char keyCode, keyCallback callback);
    void addAnyCallback(anyKeyCallback callback);
private:
    std::vector<anyKeyCallback> _anyKeyCallbacks;
	std::map<char, keyCallback> _keyCallback_map;
};

// Mouse

#define INVALID_CURSOR_POS -10.0f // indicates that cursor is off the screen
#define MAX_CURSOR_BOUND 1.0f
#define MIN_CURSOR_BOUND -1.0f

enum MOUSE_Button {
    MOUSE_LeftBtn_Down,
    MOUSE_LeftBtn_Up,
    MOUSE_RightBtn_Down,
    MOUSE_RightBtn_Up,
    /* MOUSE_MiddleBtn_Down,
    MOUSE_Scroll_Up,
    MOUSE_Scroll_Down */
};

struct Input_TracerStep {
    MOUSE_Button button;
	std::pair<float, float> step;
};

#define MAX_PATH_STEPS 4096

struct Input_TracerPath {
    Input_TracerPath(enum MOUSE_Button b){ button = b; } 

    // std::pair<float, float> getLastPath(unsigned short s){ return (s < stepsCount)? steps[stepsCount - s - 1] : steps[stepsCount - 1]; }

	MOUSE_Button button;
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

typedef void (*pressCallback)(float, float); // triggers action on a mouse button press
typedef void (*hoverCallback)(float, float); // triggers action on a cursor hover over specified region
typedef void (*dragCallback)(float, float); // triggers action on a cursor hover over specified region
typedef void (*scrollCallback)(bool); // triggers action on wheel in positive or negative direction

class Input_MouseControl : public Input_Control {
public:
    Input_MouseControl() : Input_Control(){}
    void setOnScroll(scrollCallback callback){ onScroll = callback; }
    void addCallback(enum MOUSE_Button mb, pressCallback callback);
    void addPress(enum MOUSE_Button mb, float x, float y);// mouse press with no cursor
    void addPress(enum MOUSE_Button mb); // mouse press no cursor
    void addHoverCallback(hoverCallback callback);
    void addHover(float x, float y); // checks for hover events given cursor position
    void addDragCallback(dragCallback callback);
    void addDrag(float x, float y);  // checks for drag events given cursor position
    
    scrollCallback onScroll = nullptr;
    std::pair<enum MOUSE_Button, bool> getIsMouseDown(){ return _isMouseDown; }
    // const Input_TracerStep* getLastTracerStep(unsigned short steps) const { return (steps < _tracerSteps.size())? &_tracerSteps[_tracerSteps.size() - steps - 1] : &_tracerSteps.back(); }
    const std::vector<Input_TracerStep>* getTracerSteps() const { return &_tracerSteps; }
    // const Input_TracerPath* getLastTracerPath(unsigned short steps) const { return (steps < _tracerPaths.size())? &_tracerPaths[_tracerPaths.size() - steps - 1] : &_tracerPaths.back(); }
    const std::vector<Input_TracerPath>* getTracerPaths() const { return &_tracerPaths; }
private:
    std::pair<enum MOUSE_Button, bool> _isMouseDown; // tracks state of mouse being held
    std::vector<Input_TracerStep> _tracerSteps; // tracks steps whenever mouse event and cursor pos is known 
	std::vector<Input_TracerPath> _tracerPaths; // tracks paths whenever mouse is held and moving

	std::map<MOUSE_Button, pressCallback> _mouseCallback_map;
    std::map<const Input_CursorRange*, hoverCallback> _hoverCallback_map;
    std::map<const Input_CursorRange*, dragCallback> _dragCallback_map;
};