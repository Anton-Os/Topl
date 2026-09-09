// #define INCLUDE_DATASTREAM

#define IGNORE_INPUTS

#define STAGE_COMPUTE

#include "Common.hlsl"

#include "Extended.hlsl"

void main(uint3 DTid : SV_DispatchThreadID){
    // outputBuffer.Store(DTid.x, 0); // test data assignment
}
