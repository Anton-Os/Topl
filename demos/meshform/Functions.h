// Transform Functions

Vec3f rigidTForm(Vec3f target, Vec3f amount){
    static unsigned index = 0;
    index++;
    return (index % 6 * (MESHFORM_TESS + 1) == 0)? target * amount : target;
}

Vec3f curveTForm(Vec3f target, Vec3f amount){
    static unsigned index = 0;
    index++;

    return Vec3f({
        target.data[0] * (1.0F + (sinf(fabs(1.0F - amount.data[0]) * index) * MESHFORM_CURVE)),
        target.data[1] * (1.0F + (sinf(fabs(1.0F - amount.data[1]) * index) * MESHFORM_CURVE)),
        target.data[2] * (1.0F + (sinf(fabs(1.0F - amount.data[2]) * index) * MESHFORM_CURVE))
    });
}

Vec3f pullTForm(Vec3f target, Vec3f amount){
    Vec3f diffVec = amount - target;

    return (diffVec.len() < MESHFORM_SIZE * 3)? target + (amount * 0.33) : target;
}

Geo_Vertex vertexTform(const Geo_Vertex& vertex, unsigned primID, unsigned invocation){
    Vec3f randVec = Vec3f({ (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F, (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F, (((float)rand() / (float)RAND_MAX) - 1.0F) * 0.1F });
    return Geo_Vertex(vertex.position + randVec); // test
    // return (primID == 0)? vertex : Geo_Vertex(vertex.position * (primID % 3));
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

bool cull_ball(Vec3f input, double level){ 
    // std::cout << "cull_ball: " << input.toString() << " and level is " << std::to_string(level) << std::endl;
    return true; // TODO: Modify this
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
