// Distance Functions

float getLineDistance(float2 coord, float2 p1, float2 p2){
	return abs(((p2.y - p1.y) * coord.x) - ((p2.x - p1.x) * coord.y) + (p2.x * p1.y) - (p2.y * p1.x)) / sqrt(pow(p2.y - p1.y, 2.0) + pow(p2.x - p1.x, 2.0));
}

float3 getCoordDistances(float2 coord, float2 p1, float2 p2){
	return float3(
		sqrt(pow(p2.x - p1.x, 2.0) + pow(p2.y - p1.y, 2.0)), // distance between points 1 and 2
		sqrt(pow(coord.x - p1.x, 2.0) + pow(coord.y - p1.y, 2.0)), // distance between coordinate and point 1 
		sqrt(pow(coord.x - p2.x, 2.0) + pow(coord.y - p2.y, 2.0)) // distance between coordinate and point 2
	);
}

// Cursor Functions

float4 cursorDot(float2 pos, float2 coord, float radius, float4 color){
    if (distance(pos, coord) < radius) return color;
    else return float4(color.r, color.g, color.b, 0.0);
}

float4 cursorHalo(float2 pos, float2 coord, float radius, float4 color){
    if (distance(pos, coord) > radius * 0.75 && distance(pos, coord) < radius * 1.25) return color;
    else return float4(color.r, color.g, color.b, 0.0);
}

float4 cursorCross(float2 pos, float2 coord, float radius, float4 color){
    if((abs(coord.x - pos.x) < radius * 0.5 && abs(coord.y - pos.y) < radius * 0.1) || (abs(coord.y - pos.y) < radius * 0.5 && abs(coord.x - pos.x) < radius * 0.1))
        return color;
    else return float4(color.r, color.g, color.b, 0.0);
}

// Draw Functions

bool intersectLines(float lineDist, float size, float endpointDist, float dist1, float dist2){
    return lineDist < size && dist1 < endpointDist && dist2 < endpointDist;
}

bool intersectSegments(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return (abs(dist1 - dist2) * 10) - floor(abs(dist1 - dist2) * 10) < 0.5;
    else return false;
}

bool intersectRails(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return cos(lineDist * 100) < 0;
    else return false;
}

bool intersectCmp(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return dist1 / lineDist > dist2 / dist1;
    else return false;
}

bool intersectTrig(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return sin((dist1 + dist2) * 100) > tan((dist1 + dist2) * 100);
    else return false;
}

bool intersectSwirline(float lineDist, float size, float endpointDist, float dist1, float dist2){
    if(lineDist < size && dist1 < endpointDist && dist2 < endpointDist)
        return tan(dist1 / dist2) < 0.0;
    else return false;
}

bool intersectBlobs(float lineDist, float endpointDist, float dist1, float dist2){
    return lineDist + sin(dist1) * cos(dist2) < tan(endpointDist / (dist1 + dist2)) && dist1 < endpointDist && dist2 < endpointDist;
}

bool intersectStreaks(float lineDist, float2 coord, float size, float endpointDist, float dist1, float dist2){
    return lineDist * distance(cursorPos, coord) < size && dist1 < endpointDist && dist2 < endpointDist;
}

bool intersectPow(float lineDist, float2 coord, float size, float endpointDist, float dist1, float dist2){
    return pow(lineDist, coord.x) * dist1 > pow(lineDist, coord.y) * dist2 && dist1 < endpointDist && dist2 < endpointDist;
}