#include "support_def.h"

#ifdef RASTERON_H // required library for loading images

#define ANIM_BACKGROUND 0xFFEEEEEE // overrides macro set inside Rasterons Frames.h to change background

/* struct Topl_Image { // wrapper around Rasteron_Image
    Topl_Image(const Rasteron_Image* refImg){
        image = allocNewImg(refImg->name, refImg->height, refImg->width);
        for(unsigned p = 0; p < refImg->height * refImg->width; p++)
            *(image->data + p) = *(refImg->data + p);
    }
    ~Topl_Image(){ deleteImg(image); }
private:
    Rasteron_Image* image; // underlying data
}; */

struct Topl_Frames { // Frames of dynamic images, wrapper around Rasteron_Frames
	Topl_Frames(std::string prefix, unsigned height, unsigned width, unsigned short frameCount){
		data = allocNewAnim(prefix.c_str(), height, width, frameCount);
	}
	~Topl_Frames(){ deleteAnim(data); }
	
	void addFrame(const Rasteron_Image *const refImg){
        if(frameIndex == data->frameCount){
		    frameIndex = 0; 
            replay++;
        }
        addFrameData(data, refImg, frameIndex);
        frameIndex++;
    }
	Rasteron_Image* getFrameAt(unsigned index) const { return getFrame(data, index); }

private:
    unsigned short frameIndex = 0; // keeps track of current frame for draw
    unsigned short replay = 0; // increments as scene replays itself

    Rasteron_Animation* data; // underlying data
};

#define MAX_MATERIAL_PROPERTIES 24

enum MATERIAL_Property {
    MATERIAL_Base,
    // TODO: Add more properties here
};

struct Topl_Material { // Material of static images, wrapper around Rasteron_Frames
    Topl_Material(std::string prefix, unsigned height, unsigned width){
	 	data = allocNewAnim(prefix.c_str(), height, width, MAX_MATERIAL_PROPERTIES);
	}
	~Topl_Material(){ deleteAnim(data); }
	
	void addLayer(const Rasteron_Image *const refImg, MATERIAL_Property property);
	Rasteron_Image* getLayer(MATERIAL_Property property);

private:
	Rasteron_Animation* data; // underlying data
};

#endif