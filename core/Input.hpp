
#include <vector>
#include <map>
#include <cctype>

typedef void (*keyCallback)(void); // Triggers action on a particular keypress, no arguments required

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
    // MOUSE_LeftBtn_Up,
    MOUSE_LeftBtn_Down,
    // MOUSE_RightBtn_Up,
    MOUSE_RightBtn_Down,
    MOUSE_MiddleBtn_Down,
    MOUSE_Scroll_Up,
    MOUSE_Scroll_Down
};

#define TRACER_PATH_COUNT 64 // number of iterations inside each trace member

struct Input_TracerStep {
    float xPos;
    float yPos;
};

class Input_MouseLogger {
public:
    Input_MouseLogger(){}
    void addMousePress(enum MOUSE_Button mb);
    void addCallback(enum MOUSE_Button mb, keyCallback callback);

    std::vector<Input_TracerStep[TRACER_PATH_COUNT]> _tracerPaths; // records paths traced
    std::vector<Input_TracerStep> _tracerSteps; // records points traced
private:
	std::map<MOUSE_Button, keyCallback> _mouseCallback_map;
};