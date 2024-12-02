#define IGNORE_INPUTS

#include "Common.hlsl"

ByteAddressBuffer computeBuffer : register(t0);
RWByteAddressBuffer outputBuffer : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID){
    outputBuffer.Store(DTid.x, 0); // test data assignment
}