enum CUBEGATE_Face {
    CUBEGATE_Front = 0, CUBEGATE_Back = 1, CUBEGATE_Top = 2,
    CUBEGATE_Bottom = 3, CUBEGATE_Left = 4, CUBEGATE_Right = 5,
};

struct Cubegate_Construct : public Geo_Construct<Cubegate_Construct> {
    Cubegate_Construct(Geo_Mesh* faceMesh) : Geo_Construct("cubegate", faceMesh, 6) { 
        if (CUBEGATE_TESS > 0) faceMesh->tesselate(CUBEGATE_TESS);
        init(); 
    }

    void init() override {
        _geoActors[CUBEGATE_Front].setPos({ 0.0F, 0.0F, CUBEGATE_SIZE / 2.0F });
        _geoActors[CUBEGATE_Back].setPos({ 0.0F, 0.0F, -CUBEGATE_SIZE / 2.0F });
        _geoActors[CUBEGATE_Top].setPos({ 0.0F, CUBEGATE_SIZE / 2.0F, 0.0F });
        _geoActors[CUBEGATE_Top].setRot({ 0.0F, (MATH_PI / 2.0F), 0.0F });
        _geoActors[CUBEGATE_Bottom].setPos({ 0.0F, -CUBEGATE_SIZE / 2.0F, 0.0F });
        _geoActors[CUBEGATE_Bottom].setRot({ 0.0F, (MATH_PI / 2.0F), 0.0F });
        _geoActors[CUBEGATE_Left].setPos({ -CUBEGATE_SIZE / 2.0F, 0.0F, 0.0F });
        _geoActors[CUBEGATE_Left].setRot({ 0.0F, 0.0F, (MATH_PI / 2.0F) });
        _geoActors[CUBEGATE_Right].setPos({ CUBEGATE_SIZE / 2.0F, 0.0F, 0.0F });
        _geoActors[CUBEGATE_Right].setRot({ 0.0F, 0.0F, (MATH_PI / 2.0F) });
    }

    void configure(Topl_Scene* scene) override {
        scene->addGeometry(getPrefix() + "leftFace", &_geoActors[CUBEGATE_Left]);
        scene->addGeometry(getPrefix() + "rightFace", &_geoActors[CUBEGATE_Right]);
        scene->addGeometry(getPrefix() + "topFace", &_geoActors[CUBEGATE_Top]);
        scene->addGeometry(getPrefix() + "bottomFace", &_geoActors[CUBEGATE_Bottom]);
        scene->addGeometry(getPrefix() + "frontFace", &_geoActors[CUBEGATE_Front]);
        scene->addGeometry(getPrefix() + "backFace", &_geoActors[CUBEGATE_Back]);
    }
#ifdef TOPL_ENABLE_TEXTURES
    void setTextures(Topl_Scene* scene, Sampler_2D* textures[6]) {
        scene->addTexture(getPrefix() + "leftFace", textures[CUBEGATE_Left]);
        scene->addTexture(getPrefix() + "rightFace", textures[CUBEGATE_Right]);
        scene->addTexture(getPrefix() + "topFace", textures[CUBEGATE_Top]);
        scene->addTexture(getPrefix() + "bottomFace", textures[CUBEGATE_Bottom]);
        scene->addTexture(getPrefix() + "frontFace", textures[CUBEGATE_Front]);
        scene->addTexture(getPrefix() + "backFace", textures[CUBEGATE_Back]);
    }
#endif
};