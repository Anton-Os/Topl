#include <string>

#include "support_def.h"

#include "Geo_Mesh.hpp"

#define DEFAULT_IMG_SIZE 256

// Base Image wrapper around Rasteron_Image

struct Img_Base {
    Img_Base(){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Img_Base(unsigned color){ setColorImage(color); } // Solid Constructor
    Img_Base(const std::string& filePath){ setFileImage(filePath); } // File Constructor
    Img_Base(Rasteron_Text textObj){ setTextImage( &textObj); } // Text Constructor
    Img_Base(Rasteron_Image* refImage){ setImage(refImage); } // Custom Constructor

    ~Img_Base(){ cleanup(); }

    void setColorImage(unsigned color){
		cleanup();
		image = solidImgOp({ DEFAULT_IMG_SIZE, DEFAULT_IMG_SIZE }, color);
    }
    void setFileImage(const std::string& filePath){
		cleanup();
		char newFilePath[1024];
		strcpy(&newFilePath[0], filePath.c_str());
#ifdef _WIN32
		replaceFwdSlash(&newFilePath[0]);
#endif
        image = loadImgOp(&newFilePath[0]);
    }
    void setTextImage(Rasteron_Text* textObj){
		cleanup();
		_invertFont = FONT_INVERT; // make sure font renders correctly
		image = bakeText(textObj, FONT_SIZE_MED);
    }
    void setImage(ref_image_t refImage){
		cleanup();
        image = copyImgOp(refImage);
    }
    Rasteron_Image* getImage() const { return image; }
private:
	void cleanup() {
		if (image != NULL) {
			dealloc_image(image);
			image = NULL; // reset
		}
	}

    Rasteron_Image* image = NULL; // underlying data
#endif
};

// Material based on layers
#define DEFAULT_TEX_BINDING 0 // for base texture
#define MAX_TEX_BINDINGS 8 // matches LAYER_Property enumeration

/* enum LAYER_Property {
	LAYER_Albedo = 0,
	LAYER_Height = 1,
	LAYER_Roughness = 2,
	LAYER_Opacity = 3,
	LAYER_Enviornment = 4,
	LAYER_Shadow = 5,
	LAYER_Illumination = 6,
	LAYER_Testing = 7,
}; */


struct Img_Array {
	Img_Array(){ data = alloc_queue("arrayTex", { DEFAULT_IMG_SIZE, DEFAULT_IMG_SIZE }, 8); }
	Img_Array(unsigned short count){ data = alloc_queue("arrayTex", { DEFAULT_IMG_SIZE, DEFAULT_IMG_SIZE }, count); }

	Rasteron_Queue* getQueue() const { return data; }
private:
	Rasteron_Queue* data;
};

// Volume based on slices

struct Img_Volume { 
    Img_Volume() : width(DEFAULT_IMG_SIZE), height(DEFAULT_IMG_SIZE), depth(DEFAULT_IMG_SIZE) {} // Empty Constructor
#ifdef RASTERON_H
	Img_Volume(unsigned s) : width(s), height(s), depth(s) { // Matching Lengths
		queue = alloc_queue("volumeTex", { width, height }, depth);
	}
    Img_Volume(unsigned w, unsigned h, unsigned z) : width(w), height(h), depth(z) { // Custom Lengths
		queue = alloc_queue("volumeTex", { width, height }, depth);
	}
	~Img_Volume(){ dealloc_queue(queue); }
	
	void addSlice(ref_image_t refImg, unsigned d){
		if (refImg->height == height && refImg->width == width)
			addFrameAt(queue, refImg, d);
		else return; // error

		// TODO: Create composite image
		// Rasteron_Image* compositeImg = createCompositeImg(queue);
		// volumeTexImg.setImage(compositeImg); // recreate entire volumeTex image
		// dealloc_image(compositeImg);
    }
	Rasteron_Image* getSlice(unsigned d) const { return (d < depth) ? getFrameAt(queue, d) : nullptr;}
	const Img_Base* extractVolImage() const { return &volumeTexImg; }

	unsigned getWidth() const { return width; }
	unsigned getHeight() const { return height; }
	unsigned getDepth() const { return depth; }
private:
	Img_Base volumeTexImg; // conversion from 3D texture into underlying queue
	Rasteron_Queue* queue = nullptr; // underlying data
#endif
	const unsigned width, height, depth;
};

// Heightmap wrapper around Rasteron_Heightmap

struct Img_Heightmap : public Geo_Mesh { // wrapper around Rasteron_Heightmap
    Img_Heightmap() : Geo_Mesh(0){} // Empty Constructor

#ifdef RASTERON_H // required library for loading images
    Img_Heightmap(const Rasteron_Image* refImage)
    : Geo_Mesh(refImage->height * refImage->width) {
        heightmap = loadHeightmap(refImage);
		genVertices(); genIndices();
    }

    ~Img_Heightmap(){ dealloc_heightmap(heightmap); }
#endif
private:
    void genVertices() override {
#ifdef RASTERON_H
		// TODO: Add vertex generation logic
		/*  for (unsigned r = 0; r < heightmap->height; r++)
			for (unsigned c = 0; c < heightmap->width; c++) {
				float x = (1.0f / heightmap->width) * c;
				float y = (1.0f / heightmap->height) * r;
				Vec3f pos = Vec3f({ x, y, (float)*(heightmap->data + (r * heightmap->width + c)) });
				Vec3f texcoord = Vec3f({ x, y, (float)*(heightmap->data + (r * heightmap->width + c)) });
				_vertices[(r * heightmap->width + c)] = Geo_Vertex(pos, texcoord);
			} */
#endif
	}

    void genIndices() override { return; } // No indices by default 

#ifdef RASTERON_H
    Rasteron_Heightmap* heightmap = NULL;
#endif
}; 