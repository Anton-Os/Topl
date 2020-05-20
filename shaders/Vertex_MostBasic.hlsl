cbuffer CONST_DISPLACE_BUFF : register(b0){
    float3 cPos;
}

float4 main(float4 inPos : POSITION) : SV_POSITION
{
    float4 addFourPos = { cPos.x, cPos.y, cPos.z, 0.0};
    //float4 addFourPos = { 0.1, -0.2, 0.0, 0.0};
    return inPos + addFourPos;
}