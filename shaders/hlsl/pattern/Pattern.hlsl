float3 pattern1(float3 coords, float t){
    return coords;
}

float3 pattern2(float3 coords, float t){
    return float3(
        abs(coords.x) - floor(abs(coords.x)),
        abs(coords.y) - floor(abs(coords.y)),
        abs(coords.z) - floor(abs(coords.z))
    );
}

float3 pattern3(float3 coords, float t){
    return float3(
        ceil(abs(coords.x)) - abs(coords.x),
        ceil(abs(coords.y)) - abs(coords.y),
        ceil(abs(coords.z)) - abs(coords.z)
    );
}

float3 pattern4(float3 coords, float t){
    return float3(abs(sin(coords.x)), abs(cos(coords.y)), abs(tan(coords.z)));
}

float3 pattern5(float3 coords, float t){
    return float3(abs(sin(coords.x / coords.y)), abs(cos(coords.y / coords.z)), abs(tan(coords.z / coords.x)));
}

float3 pattern6(float3 coords, float t){
    return float3(abs(sin(coords.x / coords.y)), abs(cos(coords.y / coords.z)), abs(tan(coords.z / coords.x)));
}

float3 pattern7(float3 coords, float t){
    return coords * ((t / 1000) - floor(t / 1000));
}

float3 pattern8(float3 coords, float t){
    return float3(
        (abs(coords.x) - floor(abs(coords.x))) * abs(sin(t / 1000)),
        (abs(coords.y) - floor(abs(coords.y))) * abs(cos(t / 1000)),
        (abs(coords.z) - floor(abs(coords.z))) * abs(tan(t / 1000))
    );
}

float3 pattern9(float3 coords, float t){
    return float3(
        (ceil(abs(coords.x)) - abs(coords.x)) * abs(sin(t / 1000)),
        (ceil(abs(coords.y)) - abs(coords.y)) * abs(cos(t / 1000)),
        (ceil(abs(coords.z)) - abs(coords.z)) * abs(tan(t / 1000))
    );
}
