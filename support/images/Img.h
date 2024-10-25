#include <string>
#include <thread>

#include "support_def.h"

#define DEFAULT_IMG_HEIGHT 256 * 4
#define DEFAULT_IMG_WIDTH 256 * 4

// typedef Rasteron_Image* (*imageCallback)();

struct TaggedObj { // Refresh State
	char** tag; // must update when out of date
protected:
#ifdef RASTERON_H
	~TaggedObj(){ cleanup(); }
	virtual void cleanup(){ /* puts("Img destroyed"); */ }
#endif
};

// Base Image wrapper around Rasteron_Image

struct Img_Base : public TaggedObj {
    Img_Base() : TaggedObj(){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Img_Base(unsigned color) : TaggedObj(){ setColorImage(color); } // Solid Constructor
    Img_Base(const std::string& filePath) : TaggedObj(){ setFileImage(filePath); } // File Constructor
    Img_Base(Rasteron_Text textObj) : TaggedObj(){ setTextImage( &textObj); } // Text Constructor
    Img_Base(Rasteron_Image* refImage) : TaggedObj(){ 
		setImage(refImage);
		RASTERON_DEALLOC(refImage);
	} // Custom Constructor

    void setColorImage(unsigned color){
		cleanup();
		// std::thread([this](unsigned c){ image = solidImgOp({ DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH }, c); }, color);
		image = solidImgOp({ DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH }, color);
		tag = &image->name;
    }
    void setFileImage(const std::string& filePath){
		cleanup();
        image = loadImgOp(filePath.c_str()); // TODO: This may screw up image loading
		tag = &image->name;
    }
    void setTextImage(Rasteron_Text* textObj){
		cleanup();
		image = textImgOp(textObj, FONT_SIZE_MED); // TODO: Include padding
		tag = &image->name;
    }
    void setImage(ref_image_t refImage){
		cleanup();
        image = copyImgOp(refImage);
		tag = &image->name;
    }
	// TODO: Set other image types?

    Rasteron_Image* getImage() const { return image; }
protected:
	void cleanup() override {
		if (image != NULL) {
			RASTERON_DEALLOC(image);
			image = NULL; // reset
		}
	}

    Rasteron_Image* image = NULL; // underlying queue
#endif
};

// Array texture based on multiple layers

#define DEFAULT_TEX_BINDING 0 // for base texture
#define MAX_TEX_BINDINGS 8 // matches LAYER_Property enumeration

/* enum LAYER_Property { LAYER_Albedo = 0, LAYER_Height = 1, LAYER_Roughness = 2, LAYER_Opacity = 3, LAYER_Enviornment = 4, LAYER_Shadow = 5, LAYER_Illumination = 6, LAYER_Testing = 7, }; */

struct Img_Sequence : public Img_Base {
#ifndef RASTERON_H
	Img_Sequence() : Img_Base(){}
#else
	Img_Sequence() : Img_Base(){ queue = RASTERON_QUEUE_ALLOC("arrayTex", RASTERON_SIZE(DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH), 8); }
	Img_Sequence(unsigned short count) : Img_Base(){ queue = RASTERON_QUEUE_ALLOC("arrayTex", RASTERON_SIZE(DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH), count); }

	Rasteron_Image* getImage() const {
		RASTERON_DEALLOC(image);
		// image = copyImgOp(queue_getImg(queue, floor(sequence * queue->frameCount)));
		return image;
	}

    void setSeq(double s){ sequence = s; }
	void addFrame(ref_image_t refImg, unsigned f){ queue_addImg(queue, refImg, f); }

	Rasteron_Queue* getQueue() const { return queue; } // change this?
private:
	void cleanup() override {
		if (queue != NULL) {
			RASTERON_QUEUE_DEALLOC(queue);
			queue = NULL; // reset
		}
	}

	double sequence = 0.0;
	Rasteron_Queue* queue;
#endif
};

// Volume based on slices

struct Img_Volume : public TaggedObj { 
    Img_Volume() : TaggedObj(), width(DEFAULT_IMG_WIDTH), height(DEFAULT_IMG_HEIGHT), depth(DEFAULT_IMG_WIDTH) {} // Empty Constructor
#ifdef RASTERON_H
	Img_Volume(unsigned s) : width(s), height(s), depth(s), TaggedObj() { // Matching Lengths
		queue = RASTERON_QUEUE_ALLOC("volumeTex", RASTERON_SIZE(height, width), depth);
		setData();
	}
    Img_Volume(unsigned w, unsigned h, unsigned z) : width(w), height(h), depth(z), TaggedObj() { // Custom Lengths
		queue = RASTERON_QUEUE_ALLOC("volumeTex", RASTERON_SIZE(height, width), depth);
		setData();
	}
	// Img_Volume(Rasteron_Queue* queue) : width(queue_getImg(queue, 0)->width), height(queue_getImg(queue, 0)->width), depth(queue->frameCount)
	
	void addSlice(ref_image_t refImg, unsigned d){
		if (refImg->height == height && refImg->width == width) queue_addImg(queue, refImg, d);
		else return logMessage("Slice needs to match width and height"); // error

		// Updating data internally
		unsigned imgOffset = (width * depth) * d;
		for(unsigned p = 0; p < width * depth; p++) 
			*(volumeTexImg.getImage()->data + imgOffset + p) = *(refImg->data + p);
    }
	Rasteron_Image* getSlice(unsigned d) const { return (d < depth) ? queue_getImg(queue, d) : nullptr;}
	const Img_Base* getVolumeImg() const { return &volumeTexImg; }

	unsigned getWidth() const { return width; }
	unsigned getHeight() const { return height; }
	unsigned getDepth() const { return depth; }
private:
	void setData(){
		Rasteron_Image* compositeImg = RASTERON_ALLOC("composite", height, width * depth);
		for(unsigned p = 0; p < compositeImg->width * compositeImg->height; p++){
			Rasteron_Image* sliceImg = queue_getImg(queue, p / (width * depth));
			*(compositeImg->data + p) = *(sliceImg->data + (p % (width * depth)));
		}
		volumeTexImg.setImage(compositeImg);
		RASTERON_DEALLOC(compositeImg);
	}

	void cleanup() override {
		if (queue != NULL) {
			RASTERON_QUEUE_DEALLOC(queue);
			queue = NULL; // reset
		}
	}

	Img_Base volumeTexImg; // conversion from 3D texture into underlying queue
	Rasteron_Queue* queue = nullptr; // underlying queue
#endif
	const unsigned width, height, depth;
};