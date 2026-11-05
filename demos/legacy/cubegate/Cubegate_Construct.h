#include "Geo_Construct.hpp"

enum CUBEGATE_Face {
    CUBEGATE_Front = 0, CUBEGATE_Back = 1, CUBEGATE_Top = 2,
    CUBEGATE_Bottom = 3, CUBEGATE_Left = 4, CUBEGATE_Right = 5,
};

struct Cubegate_Construct : public Geo_Construct<Cubegate_Construct> {
    Cubegate_Construct(Geo_Surface* faceMesh) : Geo_Construct(CUBEGATE_NAME, faceMesh, 6 * CUBEGATE_RECURSIONS) { 
        surface = faceMesh;
        if (CUBEGATE_TESS > 0) faceMesh->tesselate(CUBEGATE_TESS);
        init(); 
    }

    void init() override {
        for (unsigned r = 0; r < recursions; r++) {
            unsigned c = r * 6;
            _geoActors[c + CUBEGATE_Front].setPos({ 0.0F, 0.0F, surface->getRadius() / 2.0F });
            _geoActors[c + CUBEGATE_Back].setPos({ 0.0F, 0.0F, -surface->getRadius() / 2.0F });
            _geoActors[c + CUBEGATE_Top].setPos({ 0.0F, surface->getRadius() / 2.0F, 0.0F });
            _geoActors[c + CUBEGATE_Top].setRot({ 0.0F, (MATH_PI / 2.0F), 0.0F });
            _geoActors[c + CUBEGATE_Bottom].setPos({ 0.0F, -surface->getRadius() / 2.0F, 0.0F });
            _geoActors[c + CUBEGATE_Bottom].setRot({ 0.0F, (MATH_PI / 2.0F), 0.0F });
            _geoActors[c + CUBEGATE_Left].setPos({ -surface->getRadius() / 2.0F, 0.0F, 0.0F });
            _geoActors[c + CUBEGATE_Left].setRot({ 0.0F, 0.0F, (MATH_PI / 2.0F) });
            _geoActors[c + CUBEGATE_Right].setPos({ surface->getRadius() / 2.0F, 0.0F, 0.0F });
            _geoActors[c + CUBEGATE_Right].setRot({ 0.0F, 0.0F, (MATH_PI / 2.0F) });
            if (r >= 1) {
                _geoActors[c + CUBEGATE_Front].setSize(VEC_3F_ONES * (1.0F / (r + 1)));
                _geoActors[c + CUBEGATE_Back].setSize(VEC_3F_ONES * (1.0F / (r + 1)));
                _geoActors[c + CUBEGATE_Top].setSize(VEC_3F_ONES * (1.0F / (r + 1)));
                _geoActors[c + CUBEGATE_Bottom].setSize(VEC_3F_ONES * (1.0F / (r + 1)));
                _geoActors[c + CUBEGATE_Left].setSize(VEC_3F_ONES * (1.0F / (r + 1)));
                _geoActors[c + CUBEGATE_Right].setSize(VEC_3F_ONES * (1.0F / (r + 1)));
            }
        }
    }

    void configure(Topl_Scene* scene) override {
        for (unsigned r = 0; r < recursions; r++) {
            unsigned c = r * 6;
            std::string countStr = std::to_string(r + 1);
            scene->addGeometry(getPrefix() + "leftFace" + countStr, &_geoActors[c + CUBEGATE_Left]);
            scene->addGeometry(getPrefix() + "rightFace" + countStr, &_geoActors[c + CUBEGATE_Right]);
            scene->addGeometry(getPrefix() + "topFace" + countStr, &_geoActors[c + CUBEGATE_Top]);
            scene->addGeometry(getPrefix() + "bottomFace" + countStr, &_geoActors[c + CUBEGATE_Bottom]);
            scene->addGeometry(getPrefix() + "frontFace" + countStr, &_geoActors[c + CUBEGATE_Front]);
            scene->addGeometry(getPrefix() + "backFace" + countStr, &_geoActors[c + CUBEGATE_Back]);
        }
    }
#ifdef TOPL_ENABLE_TEXTURES
    void setTextures(Topl_Scene* scene, Topl_Sampler_2D* textures[6]) {
        for (unsigned r = 0; r < recursions; r++) {
            std::string countStr = std::to_string(r + 1);
            scene->addTexture(getPrefix() + "leftFace" + countStr, textures[CUBEGATE_Left]);
            scene->addTexture(getPrefix() + "rightFace" + countStr, textures[CUBEGATE_Right]);
            scene->addTexture(getPrefix() + "topFace" + countStr, textures[CUBEGATE_Top]);
            scene->addTexture(getPrefix() + "bottomFace" + countStr, textures[CUBEGATE_Bottom]);
            scene->addTexture(getPrefix() + "frontFace" + countStr, textures[CUBEGATE_Front]);
            scene->addTexture(getPrefix() + "backFace" + countStr, textures[CUBEGATE_Back]);
        }
    }
#endif

    unsigned short recursions = CUBEGATE_RECURSIONS;
    Geo_Surface* surface;
};