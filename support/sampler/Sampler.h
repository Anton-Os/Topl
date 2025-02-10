#ifndef TOPL_SAMPLER_H

#include <string>
#include <thread>

#include "support_def.h"

#define DEFAULT_SAMPLE_HEIGHT 256 * 4
#define DEFAULT_SAMPLE_WIDTH 256 * 4

// typedef Rasteron_Image* (*imageCallback)();

enum TEX_Frmt { TEX_2D, TEX_3D };
enum TEX_Mode { TEX_Wrap, TEX_Mirror, TEX_Clamp };

struct Sampler_Target { // Refresh State
    Sampler_Target(TEX_Frmt f){ format = f; }

	char** tag; // must update when out of date
	TEX_Mode mode = TEX_Wrap;
protected:
#ifdef RASTERON_H
    ~Sampler_Target(){ cleanup(); }
	virtual void cleanup(){ /* puts("Img destroyed"); */ }
#endif
	TEX_Frmt format;
};

// Base Image wrapper around Rasteron_Image

struct Sampler_2D : public Sampler_Target {
    Sampler_2D() : Sampler_Target(TEX_2D){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Sampler_2D(unsigned color) : Sampler_Target(TEX_2D){ setColorImage(color); } // Solid Constructor
    Sampler_2D(const std::string& filePath) : Sampler_Target(TEX_2D){ setFileImage(filePath); } // File Constructor
    Sampler_2D(Rasteron_Text textObj) : Sampler_Target(TEX_2D){ setTextImage( &textObj); } // Text Constructor
    Sampler_2D(Rasteron_Image* refImage) : Sampler_Target(TEX_2D){
		setImage(refImage);
		RASTERON_DEALLOC(refImage);
	} // Custom Constructor
    // ~Sampler_2D(){ cleanup(); }

    void setColorImage(unsigned color){
		cleanup();
        if(image != NULL) std::thread([this](unsigned c){ image = solidImgOp({ DEFAULT_SAMPLE_HEIGHT, DEFAULT_SAMPLE_WIDTH }, c); }, color);
        else image = solidImgOp({ DEFAULT_SAMPLE_HEIGHT, DEFAULT_SAMPLE_WIDTH }, color);
		tag = &image->name;
    }
    void setFileImage(const std::string& filePath){
		cleanup();
		if(image != NULL) std::thread([this](const std::string& s){ image = loadImgOp(s.c_str()); }, filePath);
		else image = loadImgOp(filePath.c_str());
		tag = &image->name;
    }
    void setTextImage(Rasteron_Text* textObj){
		cleanup();
		if(image != NULL) std::thread([this](Rasteron_Text* t){ image = textImgOp(t, FONT_SIZE_MED); }, textObj);
		else image = textImgOp(textObj, FONT_SIZE_MED); // TODO: Include padding
		tag = &image->name;
    }
    void setImage(ref_image_t refImage){
		cleanup();
        if(image != NULL) std::thread([this](ref_image_t r){ image = copyImgOp(r); }, refImage);
		else image = copyImgOp(refImage);
		tag = &image->name;
    }
	// TODO: Set other image types?

    Rasteron_Image* getImage() const { return image; }

	void addBorder(double size, unsigned color){
		for(unsigned p = 0; p < image->height * image->width; p++){
			double x = (1.0 / (double)image->width) * (p % image->width);
            double y = (1.0 / (double)image->height) * (p / image->width);
			if(x > 1.0 - size || x < size || y > 1.0 - size || y < size) *(image->data + p) = color;
		}
	}
protected:
	ImageSize getImgSize(){ return ImageSize({ DEFAULT_SAMPLE_HEIGHT, DEFAULT_SAMPLE_WIDTH }); }
	
	void cleanup() override {
		if (image != NULL) {
			RASTERON_DEALLOC(image);
			image = NULL; // reset
		}
	}

    Rasteron_Image* image = NULL;
#endif
};

// TODO: Create Extended Image types for Noise, Tiles, Etc...

// Array texture based on multiple layers

#define DEFAULT_TEX_BINDING 0 // for base texture
#define MAX_TEX_BINDINGS 8 // matches LAYER_Property enumeration

/* enum LAYER_Property { LAYER_Albedo = 0, LAYER_Height = 1, LAYER_Roughness = 2, LAYER_Opacity = 3, LAYER_Enviornment = 4, LAYER_Shadow = 5, LAYER_Illumination = 6, LAYER_Testing = 7, }; */

struct Sampler_Array : public Sampler_2D {
#ifndef RASTERON_H
    Sampler_Array() : Sampler_2D(){}
#else
    Sampler_Array() : Sampler_2D(){ queue = RASTERON_QUEUE_ALLOC("arrayTex", RASTERON_SIZE(DEFAULT_SAMPLE_HEIGHT, DEFAULT_SAMPLE_WIDTH), 8); }
    Sampler_Array(unsigned short count) : Sampler_2D(){ queue = RASTERON_QUEUE_ALLOC("arrayTex", RASTERON_SIZE(DEFAULT_SAMPLE_HEIGHT, DEFAULT_SAMPLE_WIDTH), count); }

	Rasteron_Image* getImage() const {
		RASTERON_DEALLOC(image);
		// image = copyImgOp(queue_getImg(queue, floor(sequence * queue->frameCount)));
		return image;
	}

    void setSeq(double s){ sequence = s; }
	void addFrame(ref_image_t refImg, unsigned f){ queue_addImg(queue, refImg, f); }

	Rasteron_Queue* getQueue() const { return queue; } // change this?
protected:
	Rasteron_Queue* queue;
private:
	void cleanup() override {
		if (queue != NULL) {
			RASTERON_QUEUE_DEALLOC(queue);
			queue = NULL; // reset
		}
	}

	double sequence = 0.0;
#endif
};

// Volume based on slices

struct Sampler_3D : public Sampler_Target {
    Sampler_3D() : Sampler_Target(TEX_3D), width(DEFAULT_SAMPLE_WIDTH), height(DEFAULT_SAMPLE_HEIGHT), depth(DEFAULT_SAMPLE_WIDTH) {} // Empty Constructor
#ifdef RASTERON_H
    Sampler_3D(unsigned s) : width(s), height(s), depth(s), Sampler_Target(TEX_3D){ setData(); } // Matching Lengths
    Sampler_3D(unsigned w, unsigned h, unsigned z) : width(w), height(h), depth(z), Sampler_Target(TEX_3D){ setData(); } // Custom Lengths
    // Sampler_3D(Rasteron_Queue* queue) : width(queue_getImg(queue, 0)->width), height(queue_getImg(queue, 0)->width), depth(queue->frameCount)
	
	void addSlice(ref_image_t refImg, unsigned d){
		if (refImg->height == height && refImg->width == width) queue_addImg(queue, refImg, d);
        else return logMessage("Slice needs to match width and height, got " + std::to_string(refImg->height) + ", " + std::to_string(refImg->width) + " but expected " + std::to_string(height) + ", " + std::to_string(width)); // error

		// Updating data internally
		unsigned imgOffset = (width * depth) * d;
		for(unsigned p = 0; p < width * depth; p++) 
			*(volumeTexImg.getImage()->data + imgOffset + p) = *(refImg->data + p);
    }
	Rasteron_Image* getSlice(unsigned d) const { return (d < depth) ? queue_getImg(queue, d) : nullptr;}
    const Sampler_2D* getVolumeImg() const { return &volumeTexImg; }

	unsigned getWidth() const { return width; }
	unsigned getHeight() const { return height; }
	unsigned getDepth() const { return depth; }
private:
	void setData(){
		if(queue == nullptr) populate(0xAAFFFFFF);
		Rasteron_Image* compositeImg = RASTERON_ALLOC("composite", height, width * depth);
		for(unsigned p = 0; p < compositeImg->width * compositeImg->height; p++){
			Rasteron_Image* sliceImg = queue_getImg(queue, p / (width * depth));
			*(compositeImg->data + p) = *(sliceImg->data + (p % (width * depth)));
		}
		volumeTexImg.setImage(compositeImg);
		RASTERON_DEALLOC(compositeImg);
	}

	void populate(unsigned color){
		if(queue != nullptr) RASTERON_QUEUE_DEALLOC(queue);
		queue = RASTERON_QUEUE_ALLOC("volumeTex", RASTERON_SIZE(height, width), depth);
		for(unsigned f = 0; f < depth; f++){
            Rasteron_Image* solidImg = solidImgOp({ DEFAULT_SAMPLE_HEIGHT, DEFAULT_SAMPLE_WIDTH }, color_level(color, (1.0 / depth) * f));
			queue_addImg(queue, solidImg, f);
			RASTERON_DEALLOC(solidImg);
		} 
	}

	void cleanup() override {
		if (queue != NULL) {
			RASTERON_QUEUE_DEALLOC(queue);
			queue = NULL; // reset
		}
	}

    Sampler_2D volumeTexImg; // conversion from 3D texture into underlying queue
	Rasteron_Queue* queue = nullptr; // underlying queue
#endif
	const unsigned width, height, depth;
};

#define TOPL_SAMPLER_H
#endif