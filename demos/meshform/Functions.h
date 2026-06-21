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

Geo_Meshlet spawn_quads(Vec3f input, unsigned count) {
    Geo_Meshlet meshlet;
    meshlet.count = 6;
    meshlet.positions = (Vec3f*)malloc(sizeof(Vec3f) * meshlet.count); // this needs to be free
    *(meshlet.positions + 0) = input + Vec3f({ 0.05F, 0.05F, 0.05f });
    *(meshlet.positions + 1) = input + Vec3f({ 0.05F, -0.05F, -0.05f });
    *(meshlet.positions + 2) = input + Vec3f({ -0.05F, 0.05F, 0.0f });
    *(meshlet.positions + 3) = input + Vec3f({ -0.05F, -0.05F, 0.0f });
    *(meshlet.positions + 4) = input + Vec3f({ 0.05F, 0.05F, 0.05f });
    *(meshlet.positions + 5) = input + Vec3f({ -0.05F, -0.05F, -0.05f });
    return meshlet;
}
