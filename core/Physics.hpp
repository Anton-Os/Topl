enum CONNECT_Type {
    CONNECT_Spring, // Oscillating force both ways
    CONNECT_Bungee, // Exerts pulling force, no push
    CONNECT_String, // Fixed maximum distance apart
    CONNECT_Rod, // Fixed distance apart
    CONNECT_Cable // Fixed min and max distance apart
};

struct Phys_Connector {
    CONNECT_Type type= CONNECT_Cable;
    double restLength = 0.5f;
    double deviation = 0.1f;
};