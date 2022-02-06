
#include <vector>
#include <map>
#include <cctype>

typedef void (*keyCallback)(void); // Triggers action on a particular keypress

class Input_KeyLogger {
public:
	Input_KeyLogger(){}
	unsigned short getCallbackCount() const { return _keyCallback_map.size(); }
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
    // TODO: Implement certain details inside ValueGen to simplify code
    Input_CursorRange(float x1, float x2, float y1, float y2){
        // clamping values
        if(x1 > 1.0f) x1 = 1.0f; else if(x1 < 0.0f) x1 = 0.0f;
        if(x2 > 1.0f) x2 = 1.0f; else if(x2 < 0.0f) x2 = 0.0f;
        if(y1 > 1.0f) y1 = 1.0f; else if(y1 < 0.0f) y1 = 0.0f;
        if(y2 > 1.0f) y2 = 1.0f; else if(y2 < 0.0f) y2 = 0.0f;

        xMin = (x1 < x2)? x1 : x2; xMax = (x1 > x2)? x1 : x2; // setting min and max x values
        yMin = (y1 < y2)? y1 : y2; yMax = (y1 > y2)? y1 : y2; // setting min and max y values
    }
    float xMin; float xMax;
    float yMin; float yMax;
};

#define TRACER_PATH_COUNT 64 // how many individual points are stored per trace

typedef void (*pressCallback)(void); // Triggers action on a mouse button press
// typedef void (*pressCallback)(float x, float y); // adds cursor position as arguments
typedef void (*hoverCallback)(void); // Triggers action on a cursor hover over specified region

class Input_MouseLogger {
public:
    Input_MouseLogger(){}
    void addMousePress(enum MOUSE_Button mb); // mouse press without position
    void addMousePress(enum MOUSE_Button mb, float x, float y); // mouse press with position
    void addCallback(enum MOUSE_Button mb, pressCallback callback);
    void addHoverCallback(const Input_CursorRange* range, hoverCallback callback);

    std::vector<Input_TracerStep> _tracerSteps; // records preses traced
    // std::vector<Input_TracerStep[TRACER_PATH_COUNT]> _tracerPaths; // records paths traced
private:
	std::map<MOUSE_Button, pressCallback> _mouseCallback_map;
    std::map<const Input_CursorRange*, hoverCallback> _hoverCallback_map;
};