// #define INCLUDE_DATASTREAM

#define IGNORE_INPUTS
#include "Common.hlsl"

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID){
    // outputBuffer.Store(DTid.x, 0); // test data assignment
}
