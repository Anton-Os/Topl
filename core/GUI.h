#include <vector>

#include "native_os_def.h"

#define DEFAULT_FG_COLOR 0xFFaaaaaa
#define DEFAULT_BK_COLOR 0xFF444444

enum UI_QuadType {
    UI_Empty,
    UI_Clickable,
    UI_Parent
};

class UI_Interactive {
public:
    // UI_Interactive((*c)()) { callback = c; }
    int fireCallback(){
        (isToggled)? isToggled = false : isToggled = true;
        return callback();
    }
    unsigned toggleColor = DEFAULT_FG_COLOR;
private:
    virtual int callback() = 0;
    bool isToggled = false;
};

class  UI_QuadPane {
public:
    UI_QuadPane(){ // Empty Pane Constructor
        type = UI_Empty;
    }
    UI_QuadPane(bool isLabel, UI_Interactive* i){
        type = UI_Clickable;
        interactCtx = i;
    }

    UI_QuadType getQuadType() { return type; }

    const char* name = ""; // can be used as a label or as an image path
    unsigned bkColor = DEFAULT_BK_COLOR;
protected:
	UI_QuadType type;
private:
    UI_Interactive* interactCtx = nullptr; // fires callback if applicable
};

struct UI_ParentQuadPane : public UI_QuadPane {
    UI_ParentQuadPane(
        const UI_QuadPane* blPane, 
        const UI_QuadPane* brPane, 
        const UI_QuadPane* tlPane, 
        const UI_QuadPane* trPane)
    : UI_QuadPane() {
        type = UI_Parent;
        botLeftPane = blPane;
        botRightPane = brPane;
        topLeftPane = tlPane;
        topRightPane = trPane;
    }

    const UI_QuadPane* botLeftPane;
    const UI_QuadPane* botRightPane;
    const UI_QuadPane* topLeftPane;
    const UI_QuadPane* topRightPane;
};

struct UI_QuadLayout {
    UI_QuadLayout(){
        // by default there is one parent with 4 empty children
        childPanes.resize(4);
        parentPanes.push_back(UI_ParentQuadPane(
            &childPanes.at(0),
            &childPanes.at(1),
            &childPanes.at(2),
            &childPanes.at(3)
        ));
    }
    std::vector<UI_ParentQuadPane> parentPanes;
    std::vector<UI_QuadPane> childPanes;
};

/* struct UI_TreeLayout {
    UI_TreeLayout(){ }

    std::vector<UI_TreePane> childPanes;
} */