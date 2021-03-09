#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <vector>

struct ValueGen {
    ValueGen(){
        srand(time(NULL)); // random value seeder
    }

    float genRandFloat(){ genRandFloat(0.0, 1.0); }
    float genRandFloat(float minBound, float maxBound);

    void appendDataToBytes(void* dataPtr, std::vector<uint8_t>* bytes);
    void assignDataToBytes(void* dataPtr, std::vector<uint8_t>* bytes);
};