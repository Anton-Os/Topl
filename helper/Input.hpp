
#include <vector>
#include <map>
#include <cctype>

#define BAD_CURSOR_POS -10.0f // indicates that cursor is off the screen
#define MAX_CURSOR_POS 1.0f
#define MIN_CURSOR_POS -1.0f

typedef void (*keyCallback)(void); // Triggers action on a particular keypress

class Input_KeyLogger {
public:
	Input_KeyLogger(){}
	void addKeyPress(char keyCode);
	void addCallback(char keyCode, keyCallback callback);
private:
	std::map<char, keyCallback> _keyCallback_map;
};

enum MOUSE_Button {
    MOUSE_LeftBtn_Down,
    MOUSE_LeftBtn_Up,
    MOUSE_RightBtn_Down,
    MOUSE_RightBtn_Up,
    /* MOUSE_MiddleBtn_Down,
    MOUSE_Scroll_Up,
    MOUSE_Scroll_Down */
};

// TracerStep is used to track press events
struct Input_TracerStep {
    MOUSE_Button button;
    float xPos;
    float yPos;
};

// CursorRange is used to trigger callbacks on mouse motion
struct Input_CursorRange {
    Input_CursorRange(float xRange[2], float yRange[2]){
        xRange[0] = clamp(xRange[0]); xRange[1] = clamp(xRange[1]); 
		yRange[0] = clamp(yRange[0]); yRange[1] = clamp(yRange[1]); // clamping values
        
        xMin = (xRange[0] < xRange[1])? xRange[0] : xRange[1]; xMax = (xRange[0] > xRange[1])? xRange[0] : xRange[1]; // setting min and max x values
        yMin = (yRange[0] < yRange[1])? yRange[0] : yRange[1]; yMax = (yRange[0] > yRange[1])? yRange[0] : yRange[1]; // setting min and max y values
    }
    static float clamp(float val){
        if(val > MAX_CURSOR_POS) val = MAX_CURSOR_POS; else if(val < MIN_CURSOR_POS) val = MIN_CURSOR_POS;
		return val;
    }
    float xMin; float xMax;
    float yMin; float yMax;
};

typedef std::pair<unsigned, unsigned> tracerPath_t; // start and ending index of tracer steps inside of a path

typedef void (*pressCallback)(float, float); // triggers action on a mouse button press
typedef void (*hoverCallback)(float, float); // triggers action on a cursor hover over specified region

class Input_MouseLogger {
public:
    Input_MouseLogger(){}
    void addCallback(enum MOUSE_Button mb, pressCallback callback);
    void addMousePress(enum MOUSE_Button mb); // mouse press
    void addMousePress(enum MOUSE_Button mb, float x, float y); // positioned mouse press
    void addHoverCallback(const Input_CursorRange* range, hoverCallback callback);
    void addHover(float x, float y); // checks for hover events given cursor position

    std::vector<Input_TracerStep> _tracerSteps; // records presses traced
    std::vector<tracerPath_t> _tracerPaths; // records paths
private:
	std::map<MOUSE_Button, pressCallback> _mouseCallback_map;
    std::map<const Input_CursorRange*, hoverCallback> _hoverCallback_map;
};