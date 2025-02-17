#include "Sampler.h"

#ifdef RASTERON_H

static float valueNoiseMod(float value){ return value; }

struct Sampler_Color : public Sampler_2D {
    Sampler_Color(unsigned color) : Sampler_2D(){ putImg(solidImgOp(getImgSize(), color)); }
};

struct Sampler_Gradient : public Sampler_2D {
    Sampler_Gradient(SIDE_Type side, unsigned color1, unsigned color2) : Sampler_2D(){ putImg(gradientImgOp(getImgSize(), side, color1, color2));}
};

struct Sampler_Map : public Sampler_2D {
    Sampler_Map(coordCallback callback) : Sampler_2D(){ putImg(mapImgOp(getImgSize(), callback)); }
};

struct Sampler_Field : public Sampler_2D {
    Sampler_Field(fieldCallback callback, unsigned count) : Sampler_2D(){
        for(unsigned c = 0; c < count; c++) 
            colorPointToTable(&table, RAND_COLOR(), (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX);
        putImg((callback != nullptr)? fieldImgOp(getImgSize(), &table, callback) : vornoiImgOp(getImgSize(), &table));
    }

    /* Sampler_Field(fieldCallback callback, std::initializer_list<ColorPoint> colorPoints) : Sampler_2D(){
        for(unsigned p = colorPoints.begin(); p != colorPoints.end(); p++)
            colorPointToTable(&table, p->color, p->x, p->y);
        putImg((callback != nullptr)? fieldImgOp(getImgSize(), &table, callback) : vornoiImgOp(getImgSize(), &table));
    } */
private:
    ColorPointTable table;
};

struct Sampler_Noise : public Sampler_2D {
    Sampler_Noise(ColorGrid colorGrid) : Sampler_2D(){
        grid = colorGrid;
        putImg(noiseExtImgOp(getImgSize(), grid, valueNoiseMod)); // replace with noiseImgOp()
    }

    Sampler_Noise(ColorGrid colorGrid, unsigned octaves) : Sampler_2D(){
        grid = colorGrid;
        putImg(noiseImgOp_octave(getImgSize(), grid, octaves));
    }
private:
    ColorGrid grid;
};

struct Sampler_Cellular : public Sampler_2D {
    Sampler_Cellular(ref_image_t refImg, nebrCallback8 callback){
        seedImg = &refImg;
        putImg(cellwiseImgOp(refImg, callback));
    }
private:
    const ref_image_t* seedImg;
}; 

/* struct Sampler_Insert : public Sampler_2D {
    // TODO: Include body
}; */

/* struct Sampler_Truschet : public Sampler_2D {
    // TODO: Include body
}; */

struct Sampler_File : public Sampler_2D {
    Sampler_File(const std::string& filePath) : Sampler_2D(){ putImg(loadImgOp(filePath.c_str())); }
};

struct Sampler_Text : public Sampler_2D {
    Sampler_Text(Rasteron_Text textObj) : Sampler_2D(){ putImg(textImgOp(&textObj, FONT_SIZE_MED)); } // TODO: Include padding
};

#endif