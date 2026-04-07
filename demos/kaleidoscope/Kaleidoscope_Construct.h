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

    void init() override;

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