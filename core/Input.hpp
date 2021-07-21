
#include <vector>
#include <map>
#include <cctype>

typedef void (*keyCallback)(void); // Triggers action on a particular keypress, no arguments required

class Input_KeyLogger {
public:
	Input_KeyLogger(){}
	unsigned short getCallbackCount() const { return mKeyCallback_map.size(); }
	void addKeyPress(char keyCode);
	void addCallback(char keyCode, keyCallback callback);
private:
	std::map<char, keyCallback> mKeyCallback_map;
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

class Input_MouseLogger {
public:
    Input_MouseLogger(){}
    // void addMouseEvent(enum MOUSE_Button mb, float xNewPos, float yNewPos); // Translates mouse button enum to keystate
    void addMousePress(enum MOUSE_Button mb);
    void addCallback(enum MOUSE_Button mb, keyCallback callback);
private:
    std::map<MOUSE_Button, keyCallback> mMouseCallback_map;
    /* float xScreenPos;
    float yScreenPos; */
};