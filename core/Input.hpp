struct KeyState {
    int keyID;
    enum KEY_Event {
        KEY_idle,
        KEY_press,
        KEY_release
    } event;
};