#include "Geo_Parametric.hpp"

void Geo_Parametric::genVertices(){
    Vec3f startVertex = Vec3f({ 0.0F, 0.0F, 0.0F });
    _vertices[0] = Geo_Vertex(vertexCallback(0, &startVertex), { 0.5f, 0.5f, 0.0f });

    if(vertexCallback != nullptr)
        for(unsigned v = 1; v < _vertices.size(); v++)
            _vertices[v] = Geo_Vertex(vertexCallback(v, &_vertices[v - 1].position), { 0.0F, 0.0F, 0.0F });
}

void Geo_Parametric::genIndices(){
    unsigned startIndex = 0;
    _indices[0] = indexCallback(0, &startIndex);

    if(indexCallback != nullptr)
        for(unsigned i = 1; i < _indices.size(); i++) 
            _indices[i] = indexCallback(i, &_indices[i - 1]);
}

Geo_ParametricSet::Geo_ParametricSet(std::initializer_list<Vec3f> pointsSet) : Geo_Parametric(pointsSet.size(), pointsSet.size()){ 
    unsigned short v = 0;
    for(auto p = pointsSet.begin(); p != pointsSet.end(); p++){
        _vertices[v] = *p;
        _indices[v] = v; // for testing
        v++;
    }
}

Geo_ParametricSet::Geo_ParametricSet(std::initializer_list<Vec3f> pointsSet, std::initializer_list<Vec3f> pathsSet) : Geo_Parametric(pointsSet.size() * pathsSet.size(), pointsSet.size() * pathsSet.size()){
    unsigned short v = 0;
    for(auto p1 = pathsSet.begin(); p1 != pathsSet.end(); p1++)
        for(auto p2 = pointsSet.begin(); p2 != pointsSet.end(); p2++){
            _vertices[v] = *p1 + *p2;
            _indices[v] = v; // for testing
            v++;
        }
}

Geo_ParametricSet::Geo_ParametricSet(std::initializer_list<Vec3f> pointsSet, float scale, unsigned short iterations) : Geo_Parametric(pointsSet.size() * iterations, pointsSet.size() * iterations){
    unsigned short v = 0;
    float adjScale = 1.0F;
    for(auto i = 0; i < iterations; i++){
        for(auto p = pointsSet.begin(); p != pointsSet.end(); p++){
            _vertices[v] = *p * adjScale;
            _indices[v] = v; // for testing
            v++;
        }
        adjScale *= scale;
    }
}