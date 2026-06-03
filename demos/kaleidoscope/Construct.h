struct Kaleidoscope_Construct : public Geo_Construct<Kaleidoscope_Construct> {
    Kaleidoscope_Construct(unsigned short div) : Geo_Construct(KALEIDOSCOPE_NAME), divisions(std::make_pair(div, div)){ init(); }

    Kaleidoscope_Construct(std::pair<unsigned short, unsigned short> divs) : Geo_Construct(KALEIDOSCOPE_NAME), divisions(divs){ init(); }

    Kaleidoscope_Construct(unsigned short div, short t) : Geo_Construct(KALEIDOSCOPE_NAME), divisions(std::make_pair(div, div)){
        tessCount = t;
        init();
    }

    Kaleidoscope_Construct(std::pair<unsigned short, unsigned short> divs, short t) : Geo_Construct(KALEIDOSCOPE_NAME), divisions(divs){
        tessCount = t;
        init();
    }

    void init() override {
        unsigned short minDivs = (divisions.first < divisions.second) ? divisions.first : divisions.second;
        unsigned short maxDivs = (divisions.first < divisions.second) ? divisions.second : divisions.first;

        for (unsigned s = 0; s < KALEIDOSCOPE_SLICES; s++) {
            float z = KALEIDOSCOPE_Z;

            Shape2D shape = { (float)rand() / (float)RAND_MAX, (minDivs == maxDivs) ? minDivs : (unsigned)(rand() % maxDivs) + minDivs };
            if (tessCount != 0) meshes[s] = new Geo_Ext2D(shape, z, (unsigned)abs(tessCount));
            else meshes[s] = new Geo_Surface(shape, z);

            if (tessCount <= 0) meshes[s]->tesselate(KALEIDOSCOPE_TESS);

            _geoActors.push_back(Geo_Actor(meshes[s]));
            _geoActors.back().setPos(Vec3f({ 0.0F, 0.0F, -1.0F + (float)((2.0F / KALEIDOSCOPE_SLICES) * s) }));

            spinFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5) * 0.025;
            sizeFactors[s] = (((float)rand() / (float)RAND_MAX) - 0.5);
        }
    }

    void configure(Topl_Scene* scene) override {
        for(unsigned s = 0; s < KALEIDOSCOPE_SLICES; s++) scene->addGeometry(getPrefix() + "slice" + std::to_string(s), &_geoActors[s]);
    }

    float getSpinFactor(unsigned short index){ return (index < KALEIDOSCOPE_SLICES)? spinFactors[index] : 0.0F; }
    float getSizeFactor(unsigned short index){ return (index < KALEIDOSCOPE_SLICES)? sizeFactors[index] : 0.0F; }
private:
    const std::pair<unsigned short, unsigned short> divisions;
    short tessCount = 0;

    // Geo_Surface* meshes[KALEIDOSCOPE_SLICES];
    Geo_Surface* meshes[KALEIDOSCOPE_SLICES];
    float spinFactors[KALEIDOSCOPE_SLICES];
    float sizeFactors[KALEIDOSCOPE_SLICES];
};