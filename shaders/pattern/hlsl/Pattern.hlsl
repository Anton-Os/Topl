float3 pattern1(float3 coords, float3 origin){
    return coords - origin;
}

float3 pattern2(float3 coords, float3 origin){
    return coords + origin;
}

float3 pattern3(float3 coords, float3 origin){
    return coords * origin;
}

float3 pattern4(float3 coords, float3 origin){
    return coords / origin;
}

float3 pattern5(float3 coords, float3 origin){
    return cross(coords, origin);
}

float3 pattern6(float3 coords, float3 origin){
    return float3(pow(coords.x, origin.x), pow(coords.y, origin.y), pow(coords.z, origin.z));
}

float3 pattern7(float3 coords, float3 origin){
    return float3(pow(origin.x, coords.x), pow(origin.y, coords.y), pow(origin.z, coords.z));
}

float3 pattern8(float3 coords, float3 origin){
    return float3(sin(coords.x * origin.x), cos(coords.y * origin.y), tan(coords.z * origin.z));
}

float3 pattern9(float3 coords, float3 origin){
    return float3(sin(coords.x / origin.y), cos(coords.y / origin.z), tan(coords.z / origin.x));
}
