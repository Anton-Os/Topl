#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <vector>

#include <Eigen/Dense>

struct ValueGen {
    ValueGen(){
        srand(time(NULL)); // random value seeder
    }

    static float genRandFloat(){ genRandFloat(0.0, 1.0); }
    static float genRandFloat(float minBound, float maxBound);

    // Padding is required parameter in appendDataToBytes
    static void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingByteCount, std::vector<uint8_t>* bytes);
    static void assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* bytes);
};