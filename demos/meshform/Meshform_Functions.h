// Transform Functions

Vec3f pullTForm(Vec3f target, Vec3f amount){
    Vec3f diffVec = amount - target;
    return (diffVec.len() < MESHFORM_SIZE * 2)? target + (amount * 0.5) : target;
}
Vec3f rigidIForm(unsigned index, Vec3f target, Vec3f amount){
    return (index % 6 * (MESHFORM_TESS + 1) == 0)? target * amount : target;
}
Geo_Vertex distanceTform(const Geo_Vertex& vertex, unsigned primID, unsigned invocation){
    float dist = vertex.position.len();
    return Geo_Vertex(vertex.position * dist); // test
    // return (primID == 0)? vertex : Geo_Vertex(vertex.position * (primID % 3));
}
Geo_Vertex midpointTform(const Geo_Vertex& vertex, const Geo_Vertex& midpoint, unsigned primID, unsigned invocation){
    Vec3f randVec = Vec3f({ (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F, (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F, (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F });
    return Geo_Vertex(midpoint.position - randVec); // test
    // return (primID != 0)? vertex : Geo_Vertex(midpoint.position);
}

// Freeform Functions

bool cull_indexed(Vec3f input, double level){ 
    static unsigned index = 0;
    index++;
    return (index % 9 == 0)? true : false;
}

Geo_Meshlet spawn_randTrigs(Vec3f input, unsigned count) {
    Geo_Meshlet meshlet;
    meshlet.count = 9;
    Vec3f randVec = (VEC_3F_RAND - Vec3f({ 0.5F, 0.5F, 0.5F })) * Vec3f({ 0.05F, 0.05F, 0.05F });
    meshlet.positions = (Vec3f*)malloc(sizeof(Vec3f) * meshlet.count); // this needs to be free
    *(meshlet.positions + 0) = (randVec.data[0] < 0.0)? input : (input * 1.1F);
    *(meshlet.positions + 1) = input + randVec;
    *(meshlet.positions + 2) = input - randVec;
    *(meshlet.positions + 3) = (randVec.data[1] < 0.0)? input : (input * 1.2F);
    *(meshlet.positions + 4) = input + (randVec * (Vec3f({ 1.0F, -1.0F, 1.0F })));
    *(meshlet.positions + 5) = input - (randVec * (Vec3f({ -1.0F, 1.0F, 1.0F })));
    *(meshlet.positions + 6) = (randVec.data[2] < 0.0)? input : (input * 1.3F);
    *(meshlet.positions + 7) = input + (randVec * (Vec3f({ 1.0F, 1.0F, -1.0F })));
    *(meshlet.positions + 8) = input - (randVec * (Vec3f({ -1.0F, -1.0F, 1.0F })));
    return meshlet;
}
