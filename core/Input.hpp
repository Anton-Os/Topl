#include <ctime>
#include <queue>

struct KeyState {
    int keyID;
    enum KEY_Event {
        KEY_press,
        KEY_dbl_press,
        KEY_release
    } event;

    clock_t timestamp;
    clock_t expiration;
};

class Input_KeyLogger {
public:
    // Returns number of "expired" Keystates
    unsigned short updateKeyStates();
private:
    std::queue<KeyState> states;
};