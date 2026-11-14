#ifndef TOPL_SAMPLER_MIX_H

#include "Topl_Sampler.h"

#ifdef TOPL_ENABLE_TEXTURES

// Mix Samplers
//
//      Images made from two or more source images, each wraps a mixing operation of Rasteron.
//      Sources are only read while constructing and are resized by Rasteron to match the first when needed.
//      Sources are passed as an image, use getImage() to pass another sampler

struct Topl_Sampler_Mix : public Topl_Sampler_2D {
    Topl_Sampler_Mix(ref_image_t image1, ref_image_t image2) : Topl_Sampler_2D(){ putImg(blendImgOp(image1, image2)); } // equal blend

    Topl_Sampler_Mix(ref_image_t image1, ref_image_t image2, mixCallback callback) : Topl_Sampler_2D(){ putImg(mixingImgOp(image1, image2, callback)); } // color for each pair

    Topl_Sampler_Mix(ref_image_t image1, ref_image_t image2, ref_image_t image3, ref_image_t image4, mixCallback4 callback) : Topl_Sampler_2D(){
        putImg(mixingExtImgOp(image1, image2, image3, image4, callback));
    }
};

struct Topl_Sampler_Fusion : public Topl_Sampler_2D { // keeps the stronger channels of both images
    Topl_Sampler_Fusion(ref_image_t image1, ref_image_t image2) : Topl_Sampler_2D(){ putImg(fusionImgOp(image1, image2)); }
};

struct Topl_Sampler_Warp : public Topl_Sampler_2D { // red and green of the domain pick where to sample from, blue adjusts brightness
    Topl_Sampler_Warp(ref_image_t refImg, ref_image_t domainImg) : Topl_Sampler_2D(){ putImg(warpingImgOp(refImg, domainImg)); }
};

struct Topl_Sampler_Insert : public Topl_Sampler_2D { // places the smaller image on top of the larger, offsets range from -1 to 1
    Topl_Sampler_Insert(ref_image_t image1, ref_image_t image2) : Topl_Sampler_2D(){ putImg(insertImgOp(image1, image2, 0.0, 0.0)); } // centered

    Topl_Sampler_Insert(ref_image_t image1, ref_image_t image2, double coordX, double coordY) : Topl_Sampler_2D(){
        putImg(insertImgOp(image1, image2, coordX, coordY));
    }
};

/* struct Topl_Sampler_Truschet : public Topl_Sampler_2D {
    // TODO: Include body
}; */

#endif

#define TOPL_SAMPLER_MIX_H
#endif
