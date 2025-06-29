#include "Geo_Vertex.hpp"

Geo_Vertex::Geo_Vertex(Vec3f p) : Geo_Pos(p){ // position constructor
    normal = getNormal(position);
    texcoord = getTexCoord(position);
    color = getColor();
}

Geo_Vertex::Geo_Vertex(Vec3f p, Vec3f t, Vec3f n, Vec3f c) : Geo_Pos(p) { // full constructor
    texcoord = t;
    normal = n;
    color = c;
}

Geo_Vertex::Geo_Vertex(const Geo_Vertex& vertex) : Geo_Pos(vertex.position) { // copy constructor
    texcoord = vertex.texcoord;
    normal = vertex.normal;
    color = vertex.color;
}

Vec3f Geo_Vertex::getNormal(Vec3f vertexPos){
    Vec3f normVec = (vertexPos[2] >= 0.0)? Vec3f({ 0.0F, 0.0F, -1.0F }) : Vec3f({ 0.0F, 0.0F, 1.0f });
    return normVec;
}

Vec3f Geo_Vertex::getTexCoord(Vec3f vertexPos){ // regular texture coordinates
    vertexPos.normalize();
    return Vec3f({ (vertexPos[0] + 1.0F) / 2.0F, 1.0F - ((vertexPos[1] + 1.0F) / 2.0F), (vertexPos[2] + 1.0F) / 2.0F });
}

Vec3f Geo_Vertex::getTexCoord(unsigned v, float z) { // square texture coordinates
    float x = ((v - 1) % 4 == 0 || (v - 1) % 4 == 1)? 1.0f : 0.0f;
    float y = ((v - 1) % 4 == 1 || (v - 1) % 4 == 2) ? 1.0f : 0.0f;
    return Vec3f({ x, y, z });
}

Vec3f Geo_Vertex::getColor(){
    static unsigned vertCount = 0;
    float attenuation = floor(vertCount / 6.0F) * (1.0F / 256.0F);

    if(position.data[0] == 0.0F && position.data[1] == 0.0F && position.data[2] == 0.0F) vertCount = 0; // reset
    else vertCount++; // increment

    if(vertCount % 6 == 0) return Vec3f({ 1.0F - attenuation, 0.0, 0.0 }); // red
    else if (vertCount % 6 == 1) return Vec3f({ 0.0, 1.0F - attenuation, 0.0 }); // green
    else if (vertCount % 6 == 2) return Vec3f({ 0.0, 0.0, 1.0F - attenuation }); // blue
    else if (vertCount % 6 == 3) return Vec3f({ 1.0F - attenuation, 1.0F - attenuation, 0.0 }); // yellow
    else if (vertCount % 6 == 4) return Vec3f({ 0.0, 1.0F - attenuation, 1.0F - attenuation }); // cyan
    else if (vertCount % 6 == 5) return Vec3f({ 1.0F - attenuation, 0.0, 1.0F - attenuation }); // magenta
    else return Vec3f({ 1.0F - attenuation, 1.0F - attenuation, 1.0F - attenuation }); // white
}

// Transform Callbacks

Geo_Vertex getMidpoint(std::initializer_list<Geo_Vertex> points){
    Geo_Vertex vertex;

    for(auto p = points.begin(); p != points.end(); p++){
        vertex.position = vertex.position + (*p).position;
        vertex.normal = /* vertex.normal + */ (*p).normal;
        vertex.texcoord = /* vertex.texcoord + */ (*p).texcoord;
        vertex.position = /* vertex.color + */ (*p).color;
    }

    // return Geo_Vertex(vertex.position * (1.0 / points.size()), vertex.texcoord * (1.0 / points.size()), vertex.normal, vertex.texcoord);
    // return Geo_Vertex(vertex.position * (1.0 / points.size()), { 0.5F, 0.5F, 0.5F }, vertex.normal, vertex.texcoord);
    return Geo_Vertex(vertex.position * (1.0 / (float)points.size()));
}