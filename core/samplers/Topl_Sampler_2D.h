#include "Topl_Sampler.h"

#ifdef TOPL_ENABLE_TEXTURES

static float rangeNoiseMod(float value){ return fabs((value - 0.5F) * 2.0F); }

// Parametric Samplers

struct Topl_Sampler_Color : public Topl_Sampler_2D {
    Topl_Sampler_Color(unsigned color) : Topl_Sampler_2D(){ putImg(solidImgOp(getImgSize(), color)); }
};

struct Topl_Sampler_Gradient : public Topl_Sampler_2D {
    Topl_Sampler_Gradient(SIDE_Type side, unsigned color1, unsigned color2) : Topl_Sampler_2D(){ putImg(gradientImgOp(getImgSize(), side, color1, color2));}
};

struct Topl_Sampler_Map : public Topl_Sampler_2D {
    Topl_Sampler_Map(coordCallback callback) : Topl_Sampler_2D(){ putImg(mapImgOp(getImgSize(), callback)); }
};

struct Topl_Sampler_Field : public Topl_Sampler_2D {
    Topl_Sampler_Field(fieldCallback callback, unsigned count) : Topl_Sampler_2D(){
        for(unsigned c = 0; c < count; c++) 
            colorPointToTable(&table, RAND_COLOR(), (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX);
        putImg((callback != nullptr)? fieldImgOp(getImgSize(), &table, callback) : vornoiImgOp(getImgSize(), &table));
    }

    /* Topl_Sampler_Field(fieldCallback callback, std::initializer_list<ColorPoint> colorPoints) : Topl_Sampler_2D(){
        for(unsigned p = colorPoints.begin(); p != colorPoints.end(); p++)
            colorPointToTable(&table, p->color, p->x, p->y);
        putImg((callback != nullptr)? fieldImgOp(getImgSize(), &table, callback) : vornoiImgOp(getImgSize(), &table));
    } */
private:
    ColorPointTable table;
};

struct Topl_Sampler_Noise : public Topl_Sampler_2D {
    Topl_Sampler_Noise(ColorGrid colorGrid) : Topl_Sampler_2D(){
        grid = colorGrid;
        putImg(noiseImgOp(getImgSize(), grid));
    }

    Topl_Sampler_Noise(ColorGrid colorGrid, noiseCallback callback) : Topl_Sampler_2D(){
        grid = colorGrid;
        putImg(noiseExtImgOp(getImgSize(), grid, callback));
    }

    Topl_Sampler_Noise(ColorGrid colorGrid, unsigned octaves) : Topl_Sampler_2D(){
        grid = colorGrid;
        putImg(noiseImgOp_octave(getImgSize(), grid, octaves));
    }
private:
    ColorGrid grid;
};

// Combinatino Samplers

struct Topl_Sampler_Cellular : public Topl_Sampler_2D {
    Topl_Sampler_Cellular(ref_image_t refImg, nebrCallback8 callback){
        seedImg = &refImg;
        putImg(cellwiseImgOp(refImg, callback));
    }
private:
    const ref_image_t* seedImg;
}; 

struct Topl_Sampler_Mix : public Topl_Sampler_2D {
    Topl_Sampler_Mix(ref_image_t image1, ref_image_t image2){ putImg(blendImgOp(image1, image2)); }

    Topl_Sampler_Mix(ref_image_t image1, ref_image_t image2, mixCallback callback){ putImg(mixImgOp(image1, image2, callback)); }
};

/* struct Topl_Sampler_Insert : public Topl_Sampler_2D {
    // TODO: Include body
}; */

/* struct Topl_Sampler_Truschet : public Topl_Sampler_2D {
    // TODO: Include body
}; */

// Asset Samplers

struct Topl_Sampler_File : public Topl_Sampler_2D {
    Topl_Sampler_File(const std::string& filePath) : Topl_Sampler_2D(){ 
        putImg(loadImgOp(filePath.c_str()));
        // for(unsigned p = 0; p < image->width * image->height; p++) *(image->data + p) = swap_rb(*(image->data + p)); // TODO: Flip color bits
    }
};

struct Topl_Sampler_Text : public Topl_Sampler_2D {
#if RASTERON_ENABLE_FONT
    Topl_Sampler_Text(Rasteron_Text textObj) : Topl_Sampler_2D(){ putImg(textImgOp(&textObj, FONT_SIZE_MED)); } // TODO: Include padding?

    Topl_Sampler_Text(Rasteron_Message messageObj) : Topl_Sampler_2D(){ putImg(messageImgOp(&messageObj, FONT_SIZE_MED)); } // TODO: Include padding
#endif
};

#endif
