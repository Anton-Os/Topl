#include <string>

#include "support_def.h"


#define DEFAULT_IMG_HEIGHT 256
#define DEFAULT_IMG_WIDTH 256

struct Img { // Refresh State
	void setRefresh(bool value){ if(isRefresh != nullptr) *isRefresh = value; }
	bool* isRefresh; // this updates whenever refresh is required 
protected:
#ifdef RASTERON_H
	~Img(){ cleanup(); }
	virtual void cleanup(){ /* puts("Img destroyed"); */ }
#endif
};

// Base Image wrapper around Rasteron_Image

struct Img_Base : public Img {
    Img_Base() : Img(){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Img_Base(unsigned color) : Img(){ setColorImage(color); } // Solid Constructor
    Img_Base(const std::string& filePath) : Img(){ setFileImage(filePath); } // File Constructor
    Img_Base(Rasteron_Text textObj) : Img(){ setTextImage( &textObj); } // Text Constructor
    Img_Base(Rasteron_Image* refImage) : Img(){ 
		setImage(refImage);
		RASTERON_DEALLOC(refImage);
	} // Custom Constructor

    void setColorImage(unsigned color){
		refresh();
		image = solidImgOp({ DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH }, color);
    }
    void setFileImage(const std::string& filePath){
		refresh();
		char newFilePath[1024];
		strcpy(&newFilePath[0], filePath.c_str());
#ifdef _WIN32
		replaceFwdSlash(&newFilePath[0]);
#endif
        image = loadImgOp(&newFilePath[0]);
    }
    void setTextImage(Rasteron_Text* textObj){
		refresh();
		image = textImgOp(textObj, FONT_SIZE_MED); // TODO: Include padding
    }
    void setImage(ref_image_t refImage){
		refresh();
        image = copyImgOp(refImage);
    }
    Rasteron_Image* getImage() const { return image; }
protected:
	void refresh() {
		cleanup();
		setRefresh(true);
	}
	void cleanup() override {
		// puts("Img_Base destroyed");
		if (image != NULL) {
			RASTERON_DEALLOC(image);
			image = NULL; // reset
		}
	}

    Rasteron_Image* image = NULL; // underlying queue
#endif
};

/* struct Img_Txt : public Img_Base {
	Img_Txt() : Img_Base(){} // Empty Constructor
#ifdef RASTERON_H
	Img_Txt(Rasteron_Text* text) : Img_Base(){
		textObj = text;
		image = textImgOp(textObj, size);
	}
	// Img_Txt(Rasteron_Text* text, unsigned short l, unsigned short r, unsigned short t, unsigned short b){
	// 	textObj = text;
	//	leftPad = l; rightPad = r;
	//	topPad = t; botPad = b;

	//	unsigned short paddings[4] = { l, r, t, b };
	//	image = textPadImgOp(textObj, size, paddings);
	// }

	// void setStr(std::string textStr);
	// void setFontFile(std::string fontStr);
	// void setColors(unsigned fgColor, unsigned bkColor);

	unsigned short size = FONT_SIZE_MED;

	unsigned short leftPad = 0; unsigned short rightPad = 0;
	unsigned short topPad = 0; unsigned short botPad = 0;
protected:
	Rasteron_Text* textObj;
#endif
}; */

// Array texture based on multiple layers

#define DEFAULT_TEX_BINDING 0 // for base texture
#define MAX_TEX_BINDINGS 8 // matches LAYER_Property enumeration

/* enum LAYER_Property { LAYER_Albedo = 0, LAYER_Height = 1, LAYER_Roughness = 2, LAYER_Opacity = 3, LAYER_Enviornment = 4, LAYER_Shadow = 5, LAYER_Illumination = 6, LAYER_Testing = 7, }; */

struct Img_Array : public Img {
#ifndef RASTERON_H
	Img_Array(){}
#else
	Img_Array() : Img(){ queue = RASTERON_QUEUE_ALLOC("arrayTex", RASTERON_SIZE(DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH), 8); }
	Img_Array(unsigned short count) : Img(){ queue = RASTERON_QUEUE_ALLOC("arrayTex", RASTERON_SIZE(DEFAULT_IMG_HEIGHT, DEFAULT_IMG_WIDTH), count); }

    // void addImg();
	Rasteron_Queue* getQueue() const { return queue; } // change this?
private:
	void cleanup() override {
		if (queue != NULL) {
			RASTERON_QUEUE_DEALLOC(queue);
			queue = NULL; // reset
		}
	}

	Rasteron_Queue* queue;
#endif
};

// Volume based on slices

struct Img_Volume : public Img { 
    Img_Volume() : width(DEFAULT_IMG_WIDTH), height(DEFAULT_IMG_HEIGHT), depth(DEFAULT_IMG_WIDTH), Img() {} // Empty Constructor
#ifdef RASTERON_H
	Img_Volume(unsigned s) : width(s), height(s), depth(s), Img() { // Matching Lengths
		queue = RASTERON_QUEUE_ALLOC("volumeTex", RASTERON_SIZE(height, width), depth);
		setData();
	}
    Img_Volume(unsigned w, unsigned h, unsigned z) : width(w), height(h), depth(z), Img() { // Custom Lengths
		queue = RASTERON_QUEUE_ALLOC("volumeTex", RASTERON_SIZE(height, width), depth);
		setData();
	}
	// Img_Volume(Rasteron_Queue* queue) : width(queue_getImg(queue, 0)->width), height(queue_getImg(queue, 0)->width), depth(queue->frameCount)
	
	void addSlice(ref_image_t refImg, unsigned d){
        setRefresh(true);

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