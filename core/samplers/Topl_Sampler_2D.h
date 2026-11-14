#ifndef TOPL_SAMPLER_2D_H

#include <initializer_list>

#include "Topl_Sampler.h"

#ifdef TOPL_ENABLE_TEXTURES

static float rangeNoiseMod(float value){ return fabs((value - 0.5F) * 2.0F); }

// Parametric Samplers
//
//      Images generated from parameters alone, each wraps a procedural or noise operation of Rasteron

struct Topl_Sampler_Color : public Topl_Sampler_2D {
    Topl_Sampler_Color(unsigned color) : Topl_Sampler_2D(){ putImg(solidImgOp(getImgSize(), color)); }
};

struct Topl_Sampler_Gradient : public Topl_Sampler_2D {
    Topl_Sampler_Gradient(SIDE_Type side, unsigned color1, unsigned color2) : Topl_Sampler_2D(){ putImg(gradientImgOp(getImgSize(), side, color1, color2));}
};

struct Topl_Sampler_Lines : public Topl_Sampler_2D { // stripes across the image, in columns unless rows are requested
    Topl_Sampler_Lines(unsigned color1, unsigned color2, unsigned short divs) : Topl_Sampler_2D(){ putImg(linedImgOp(getImgSize(), color1, color2, divs, 0.0)); }

    Topl_Sampler_Lines(unsigned color1, unsigned color2, unsigned short divs, bool isRows) : Topl_Sampler_2D(){ putImg(linedImgOp(getImgSize(), color1, color2, divs, (isRows)? 1.0 : 0.0)); }
};

struct Topl_Sampler_Checker : public Topl_Sampler_2D { // alternating cells of two colors
    Topl_Sampler_Checker(ColorGrid colorGrid) : Topl_Sampler_2D(){ putImg(checkeredImgOp(getImgSize(), colorGrid)); }

    Topl_Sampler_Checker(unsigned short cells, unsigned color1, unsigned color2) : Topl_Sampler_2D(){
        ColorGrid colorGrid = { cells, cells, color1, color2 };
        putImg(checkeredImgOp(getImgSize(), colorGrid));
    }
};

struct Topl_Sampler_Map : public Topl_Sampler_2D {
    Topl_Sampler_Map(coordCallback callback) : Topl_Sampler_2D(){ putImg(mapImgOp(getImgSize(), callback)); }
};

struct Topl_Sampler_Field : public Topl_Sampler_2D { // colors from the nearest points, vornoi cells without a callback
    Topl_Sampler_Field(fieldCallback callback, unsigned count) : Topl_Sampler_2D(){ // random points
        ColorPointTable* table = genTable(count);
        putImg((callback != nullptr)? fieldImgOp(getImgSize(), table, callback) : vornoiImgOp(getImgSize(), table));
        free(table);
    }

    Topl_Sampler_Field(fieldCallback3 callback, unsigned count) : Topl_Sampler_2D(){ // random points, callback receives the 3 nearest
        ColorPointTable* table = genTable(count);
        putImg(fieldExtImgOp(getImgSize(), table, callback));
        free(table);
    }

    Topl_Sampler_Field(fieldCallback callback, std::initializer_list<ColorPoint> colorPoints) : Topl_Sampler_2D(){
        ColorPointTable* table = genTable(colorPoints);
        putImg((callback != nullptr)? fieldImgOp(getImgSize(), table, callback) : vornoiImgOp(getImgSize(), table));
        free(table);
    }

    Topl_Sampler_Field(fieldCallback3 callback, std::initializer_list<ColorPoint> colorPoints) : Topl_Sampler_2D(){
        ColorPointTable* table = genTable(colorPoints);
        putImg(fieldExtImgOp(getImgSize(), table, callback));
        free(table);
    }
private:
    // tables hold up to MAX_PIXELPOINTS points, which is too large to keep on the stack or inside of the sampler
    static ColorPointTable* genTable(unsigned count){
        ColorPointTable* table = (ColorPointTable*)calloc(1, sizeof(ColorPointTable));
        for(unsigned c = 0; c < count; c++)
            colorPointToTable(table, RAND_COLOR(), (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX);
        return table;
    }

    static ColorPointTable* genTable(std::initializer_list<ColorPoint> colorPoints){
        ColorPointTable* table = (ColorPointTable*)calloc(1, sizeof(ColorPointTable));
        for(auto p = colorPoints.begin(); p != colorPoints.end(); p++) colorPointToTable(table, p->color, p->x, p->y);
        return table;
    }
};

enum NOISE_Type {
    NOISE_Value = 0, // smooth blend between the cells of the grid
    NOISE_Tiled = 1, // discrete tiles
    NOISE_Crossed = 2, // scratch effects
    NOISE_Stepped = 3 // step effects
};

enum NOISE_Octaves { // how the octaves of a noise are combined
    NOISE_Brown = 0, // octaves blended equally
    NOISE_Pink = 1, // octaves fused towards the low colors
    NOISE_Blue = 2, // octaves fused towards the high colors
    NOISE_Random = 3, // octaves fused by random amounts
    NOISE_Added = 4,
    NOISE_Subtracted = 5,
    NOISE_Multiplied = 6
};

struct Topl_Sampler_Noise : public Topl_Sampler_2D {
    Topl_Sampler_Noise(ColorGrid colorGrid) : Topl_Sampler_2D(){ putImg(noiseImgOp(getImgSize(), colorGrid)); }

    Topl_Sampler_Noise(ColorGrid colorGrid, noiseCallback callback) : Topl_Sampler_2D(){ putImg(noiseExtImgOp(getImgSize(), colorGrid, callback)); }

    Topl_Sampler_Noise(ColorGrid colorGrid, unsigned octaves) : Topl_Sampler_2D(){ putImg(noiseImgOp_octave(getImgSize(), colorGrid, octaves)); }

    Topl_Sampler_Noise(ColorGrid colorGrid, NOISE_Type type) : Topl_Sampler_2D(){
        switch(type){
            case NOISE_Tiled: putImg(noiseImgOp_tiled(getImgSize(), colorGrid)); break;
            case NOISE_Crossed: putImg(noiseImgOp_crossed(getImgSize(), colorGrid)); break;
            case NOISE_Stepped: putImg(noiseImgOp_stepped(getImgSize(), colorGrid)); break;
            default: putImg(noiseImgOp(getImgSize(), colorGrid)); break;
        }
    }

    Topl_Sampler_Noise(ColorGrid colorGrid, unsigned short octaves, NOISE_Octaves type) : Topl_Sampler_2D(){
        switch(type){
            case NOISE_Pink: putImg(noiseImgOp_low(getImgSize(), colorGrid, octaves)); break;
            case NOISE_Blue: putImg(noiseImgOp_hi(getImgSize(), colorGrid, octaves)); break;
            case NOISE_Random: putImg(noiseImgOp_rand(getImgSize(), colorGrid, octaves)); break;
            case NOISE_Added: putImg(noiseImgOp_add(getImgSize(), colorGrid, octaves)); break;
            case NOISE_Subtracted: putImg(noiseImgOp_diff(getImgSize(), colorGrid, octaves)); break;
            case NOISE_Multiplied: putImg(noiseImgOp_mult(getImgSize(), colorGrid, octaves)); break;
            default: putImg(noiseImgOp_octave(getImgSize(), colorGrid, octaves)); break;
        }
    }

    Topl_Sampler_Noise(ColorGrid colorGrid, unsigned short octaves, mixCallback callback) : Topl_Sampler_2D(){ // octaves combined by the callback
        putImg(noiseExtImgOp_octave(getImgSize(), colorGrid, octaves, callback));
    }

    Topl_Sampler_Noise(unsigned color1, unsigned color2) : Topl_Sampler_2D(){ putImg(noiseImgOp_white(getImgSize(), color1, color2)); } // white noise, random per pixel
};

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

#include "Topl_Sampler_Filter.h" // samplers made from another image
#include "Topl_Sampler_Mix.h" // samplers made from several images

#define TOPL_SAMPLER_2D_H
#endif
