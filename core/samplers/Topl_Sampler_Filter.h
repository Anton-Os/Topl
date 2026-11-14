#ifndef TOPL_SAMPLER_FILTER_H

#include "Topl_Sampler.h"

#ifdef TOPL_ENABLE_TEXTURES

// Filter Samplers
//
//      Images made from a single source image, each wraps a filtering, basic, or cellwise operation of Rasteron.
//      The source is only read while constructing, so the result is independent from it afterwards.
//      Sources are passed as an image, use getImage() to pass another sampler

struct Topl_Sampler_Grey : public Topl_Sampler_2D { // greyscale
    Topl_Sampler_Grey(ref_image_t refImg) : Topl_Sampler_2D(){ putImg(greyImgOp(refImg)); }
};

struct Topl_Sampler_Channel : public Topl_Sampler_2D { // single color channel
    Topl_Sampler_Channel(ref_image_t refImg, CHANNEL_Type channel) : Topl_Sampler_2D(){ putImg(filterImgOp(refImg, channel)); } // other channels are removed

    Topl_Sampler_Channel(ref_image_t refImg, CHANNEL_Type channel, bool isAveraged) : Topl_Sampler_2D(){ // averaged spreads the channel to every channel
        putImg((isAveraged)? channelImgOp(refImg, channel) : filterImgOp(refImg, channel));
    }
};

struct Topl_Sampler_Split : public Topl_Sampler_2D { // colors reduced to a number of levels
    Topl_Sampler_Split(ref_image_t refImg, unsigned short levels) : Topl_Sampler_2D(){ putImg(splitImgOp(refImg, levels)); }
};

struct Topl_Sampler_Recolor : public Topl_Sampler_2D {
    Topl_Sampler_Recolor(ref_image_t refImg, recolorCallback callback) : Topl_Sampler_2D(){ putImg(recolorImgOp(refImg, callback)); } // color for each color

    Topl_Sampler_Recolor(ref_image_t refImg, remaskCallback callback) : Topl_Sampler_2D(){ putImg(remaskImgOp(refImg, callback)); } // value for each channel

    Topl_Sampler_Recolor(ref_image_t refImg, CHANNEL_Type channel1, CHANNEL_Type channel2) : Topl_Sampler_2D(){ // two channels switched
        putImg(colorSwitchImgOp(refImg, channel1, channel2));
    }
};

// Transform Samplers

struct Topl_Sampler_Resize : public Topl_Sampler_2D {
    Topl_Sampler_Resize(ref_image_t refImg) : Topl_Sampler_2D(){ putImg(resizeImgOp(getImgSize(), refImg)); } // default sampler size

    Topl_Sampler_Resize(ref_image_t refImg, unsigned height, unsigned width) : Topl_Sampler_2D(){ putImg(resizeImgOp(ImageSize({ height, width }), refImg)); }
};

struct Topl_Sampler_Crop : public Topl_Sampler_2D {
    Topl_Sampler_Crop(ref_image_t refImg, SIDE_Type side, double factor) : Topl_Sampler_2D(){ putImg(cropImgOp(refImg, side, factor)); } // removes a fraction from a side

    Topl_Sampler_Crop(ref_image_t refImg, double topLeft, double topRight, double botLeft, double botRight) : Topl_Sampler_2D(){ // cuts into each corner
        putImg(cornerImgOp(refImg, topLeft, topRight, botLeft, botRight));
    }
};

struct Topl_Sampler_Flip : public Topl_Sampler_2D {
    Topl_Sampler_Flip(ref_image_t refImg) : Topl_Sampler_2D(){ putImg(mirrorImgOp(refImg)); } // horizontal mirror

    Topl_Sampler_Flip(ref_image_t refImg, FLIP_Type type) : Topl_Sampler_2D(){ putImg(flipImgOp(refImg, type)); } // quarter and half turns
};

// Cellwise Samplers
//
//      Each pixel is computed from itself and adjacent pixels, callbacks return NO_COLOR to keep a pixel

enum CELL_Type { CELL_Rows = 0, CELL_Columns = 1 }; // direction that an image grows from its first row or column

struct Topl_Sampler_Cellular : public Topl_Sampler_2D {
    Topl_Sampler_Cellular(ref_image_t refImg, nebrCallback8 callback) : Topl_Sampler_2D(){ putImg(cellwiseImgOp(refImg, callback)); }

    Topl_Sampler_Cellular(ref_image_t refImg, nebrCallback8 callback, unsigned short iterations) : Topl_Sampler_2D(){ // each iteration starts from the last
        putImg(cellwiseExtImgOp(refImg, callback, iterations));
    }

    Topl_Sampler_Cellular(ref_image_t refImg, nebrCallback2 callback, CELL_Type type) : Topl_Sampler_2D(){ // each row or column is computed from the one before it
        putImg((type == CELL_Columns)? cellwiseColImgOp(refImg, callback) : cellwiseRowImgOp(refImg, callback));
    }
};

struct Topl_Sampler_Antialias : public Topl_Sampler_2D { // averages each pixel with adjacent pixels
    Topl_Sampler_Antialias(ref_image_t refImg) : Topl_Sampler_2D(){ putImg(antialiasImgOp(refImg, 1)); }

    Topl_Sampler_Antialias(ref_image_t refImg, unsigned short times) : Topl_Sampler_2D(){ putImg(antialiasImgOp(refImg, times)); }
};

#endif

#define TOPL_SAMPLER_FILTER_H
#endif
