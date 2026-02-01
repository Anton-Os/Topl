#ifndef TOPL_SAMPLER_H

#include <string>
#include <thread>

#include "support_def.h"

#define SAMPLER_HEIGHT 256 * 4
#define SAMPLER_WIDTH 256 * 4

// typedef Rasteron_Image* (*imageCallback)();

enum TEX_Frmt { TEX_2D, TEX_3D };
enum TEX_Mode { TEX_Wrap, TEX_Mirror, TEX_Clamp };

struct Sampler_Target { // Refresh State
    Sampler_Target(TEX_Frmt f){ 
		format = f;
#ifdef TOPL_ENABLE_TEXTURES
		_invertImage = INVERT_IMG_TRUE; // Imaging Initialization
#endif
	}

	char** tag; // must update when out of date
	TEX_Mode mode = TEX_Wrap;
protected:
#ifdef TOPL_ENABLE_TEXTURES
    ~Sampler_Target(){ cleanup(); }
	virtual void cleanup(){ /* puts("Img destroyed"); */ }
#endif
	TEX_Frmt format;
};

// Base Image wrapper around Rasteron_Image

struct Sampler_2D : public Sampler_Target {
    Sampler_2D() : Sampler_Target(TEX_2D){} // Empty Constructor
	// Sampler_2D(const char* name, unsigned width, unsigned height) : Sampler_Target(TEX_2D){}
#ifdef TOPL_ENABLE_TEXTURES // required library for loading images
    Sampler_2D(Rasteron_Image* refImage) : Sampler_Target(TEX_2D){
		setImage(refImage);
		RASTERON_DEALLOC(refImage);
	} // Custom Constructor
    // ~Sampler_2D(){ cleanup(); }

    void setImage(ref_image_t refImage){
		cleanup();
        if(image != NULL) std::thread([this](ref_image_t r){ image = copyImgOp(r); }, refImage);
		else image = copyImgOp(refImage);
		tag = &image->name;
    }
	void putImg(Rasteron_Image* targetImg){
		cleanup();
		image = targetImg;
		tag = &image->name;
	}

    Rasteron_Image* getImage() const { return image; }

	void addBorder(double size, unsigned color){
		for(unsigned p = 0; p < image->height * image->width; p++){
			double x = (1.0 / (double)image->width) * (p % image->width);
            double y = (1.0 / (double)image->height) * (p / image->width);
			if(x > 1.0 - size || x < size || y > 1.0 - size || y < size) *(image->data + p) = color;
		}
	}
protected:
	ImageSize getImgSize(){ return ImageSize({ SAMPLER_HEIGHT, SAMPLER_WIDTH }); }
	
	void cleanup() override {
		if (image != NULL) {
			RASTERON_DEALLOC(image);
			image = NULL; // reset
		}
	}

    Rasteron_Image* image = solidImgOp(getImgSize(), 0xFFEEEEEE);
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
    Sampler_Array() : Sampler_2D(){ queue = RASTERON_QUEUE_ALLOC("arrayTex", RASTERON_SIZE(SAMPLER_HEIGHT, SAMPLER_WIDTH), 8); }
    Sampler_Array(unsigned short count) : Sampler_2D(){ queue = RASTERON_QUEUE_ALLOC("arrayTex", RASTERON_SIZE(SAMPLER_HEIGHT, SAMPLER_WIDTH), count); }

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

#define SAMPLER_VOLUME_BACKGROUND 0xFFEEEEEE
#define SAMPLER_COLOR_QUEUE nullptr

struct Sampler_3D : public Sampler_Target {
    Sampler_3D() : Sampler_Target(TEX_3D), width(SAMPLER_WIDTH), height(SAMPLER_HEIGHT), depth(SAMPLER_WIDTH) {} // Empty Constructor
#ifdef TOPL_ENABLE_TEXTURES
    Sampler_3D(unsigned s) : width(s), height(s), depth(s), Sampler_Target(TEX_3D){ setData(SAMPLER_COLOR_QUEUE); } // Matching Lengths
    Sampler_3D(unsigned w, unsigned h, unsigned z) : width(w), height(h), depth(z), Sampler_Target(TEX_3D){ setData(SAMPLER_COLOR_QUEUE); } // Custom Lengths
    Sampler_3D(Rasteron_Queue* q) : width(queue_getImg(queue, 0)->width), height(queue_getImg(queue, 0)->height), depth(queue->frameCount), Sampler_Target(TEX_3D){
		assert(width == height);
		setData(q);
	}
	
	Rasteron_Image* getSlice(unsigned d) const { return (d < depth) ? queue_getImg(queue, d) : nullptr;}
    const Sampler_2D* getVolumeImg() const { return &volumeTexImg; }

	void addSlice(ref_image_t refImg, unsigned d){
		if (refImg->height == height && refImg->width == width) queue_addImg(queue, refImg, d);
        else return logMessage("Slice needs to match width and height, got " + std::to_string(refImg->height) + ", " + std::to_string(refImg->width) + " but expected " + std::to_string(height) + ", " + std::to_string(width)); // error

		// Updating data internally
		unsigned imgOffset = (width * depth) * d;
		for(unsigned p = 0; p < width * depth; p++) 
			*(volumeTexImg.getImage()->data + imgOffset + p) = *(refImg->data + p);
    }
	void setColors(unsigned color){ setColors(color, color_invert(color)); }
	void setColors(unsigned color1, unsigned color2){
		if(queue != nullptr) RASTERON_QUEUE_DEALLOC(queue);
		std::string texName = "volumeTex_" + std::to_string(color1) + "," + std::to_string(color2);
		queue = RASTERON_QUEUE_ALLOC(texName.c_str(), RASTERON_SIZE(height, width), depth);
		for(unsigned f = 0; f < depth; f++){
			Rasteron_Image* colorImg = solidImgOp({ SAMPLER_HEIGHT, SAMPLER_WIDTH }, blend_colors(color1, color2, (1.0 / depth) * f));
			queue_addImg(queue, colorImg, f);
			RASTERON_DEALLOC(colorImg);
		}
		tag = &queue->prefix;
	}

	unsigned getWidth() const { return width; }
	unsigned getHeight() const { return height; }
	unsigned getDepth() const { return depth; }
private:
	void setData(Rasteron_Queue* q){
		if(q != SAMPLER_COLOR_QUEUE) queue = q; 
		else setColors(RAND_COLOR());
		// else setColors(SAMPLER_VOLUME_BACKGROUND);
		
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

    Sampler_2D volumeTexImg; // conversion from 3D texture into underlying queue
	Rasteron_Queue* queue = nullptr; // underlying queue
#endif
	const unsigned width, height, depth;
};

#define TOPL_SAMPLER_H
#endif