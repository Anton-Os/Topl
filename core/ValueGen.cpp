#include "ValueGen.hpp"

void ValueGen::appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingByteCount, std::vector<uint8_t>* bytes){
    for(unsigned d = 0; d < dataSize + paddingByteCount; d++)
        (d < dataSize && data_ptr + d != nullptr)
            ? bytes->push_back(*(data_ptr + d)) // Value is copied into bytes
            : bytes->push_back(0); // Otherwise padding is created
}

void ValueGen::assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* bytes){
    bytes->clear();
    bytes->resize(dataSize);
    
    for(unsigned d = 0; d < dataSize; d++)
        if(data_ptr + d != nullptr) bytes->at(d) = *(data_ptr + d);
}