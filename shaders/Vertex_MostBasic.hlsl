cbuffer CONST_DISPLACE_BUFF : register(b0){
    float3 cPos;
}

float4 main(
    float4 inPos : POSITION,
    float2 inTexcoord : TEXCOORD,
) : SV_POSITION { // Only output is position
    float4 addFourPos = { cPos.x, cPos.y, cPos.z, 0.0};
    return inPos + addFourPos;
}