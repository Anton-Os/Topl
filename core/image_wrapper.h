#include <string>

#include "support_def.h"

#include "Geo_Mesh.hpp"

#define IMG_SIDE_LEN 256

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
		image = createSolidImg({ IMG_SIDE_LEN, IMG_SIDE_LEN }, color);
    }
    void setFileImage(const std::string& filePath){
		cleanup();
		char newFilePath[1024];
		strcpy(&newFilePath[0], filePath.c_str());
#ifdef _WIN32
		replaceFwdSlash(&newFilePath[0]);
#endif
        image = createRefImg(&newFilePath[0]);
    }
    void setTextImage(Rasteron_Text* textObj){
		cleanup();
		image = bakeTextI(textObj);
    }
    void setImage(Rasteron_Image* refImage){
		cleanup();
        image = createCopyImg(refImage);
    }
    Rasteron_Image* getImage() const { return image; }
private:
	void cleanup() {
		if (image != NULL) {
			free_image(image);
			image = NULL; // reset
		}
	}

    Rasteron_Image* image = NULL; // underlying data
#endif
};

// Material based on layers

#define MAX_TEX_BINDINGS 6 // matches MATERIAL_Property enumeration

/* enum MATERIAL_Property {
	MATERIAL_Albedo = 0,
	MATERIAL_Height = 1,
	MATERIAL_Roughness = 2,
	MATERIAL_Opacity = 3,
	MATERIAL_Enviornment = 4,
	MATERIAL_Shadow = 5,
}; */

struct Img_Layer : public Img_Base {
	Img_Layer() : Img_Base(){ layer = 0; }
	Img_Layer(unsigned short l) : Img_Base(){ layer = l; }

	unsigned short layer;
};

struct Img_Material {
	const Img_Layer* getLayer(unsigned short l) const { return &layers[l]; }

	Img_Layer layers[MAX_TEX_BINDINGS] = { Img_Layer(0), Img_Layer(1), Img_Layer(2), Img_Layer(3), Img_Layer(4), Img_Layer(5) };
};

// Volume based on slices

struct Img_Volume { 
    Img_Volume() : width(IMG_SIDE_LEN), height(IMG_SIDE_LEN), depth(IMG_SIDE_LEN) {} // Empty Constructor
#ifdef RASTERON_H
	Img_Volume(unsigned s) : width(s), height(s), depth(s) { // Matching Lengths
		data = allocNewAnim("volume", depth);
	}
    Img_Volume(unsigned w, unsigned h, unsigned z) : width(w), height(h), depth(z) { // Custom Lengths
		data = allocNewAnim("volume", depth);
	}
	~Img_Volume(){ deleteAnim(data); }
	
	void addSlice(ref_image_t refImg, unsigned d){
		if (refImg->height == height && refImg->width == width)
			addFrameData(data, refImg, d);
		else return; // error

		Rasteron_Image* compositeImg = createCompositeImg(data);
		volumeImg.setImage(compositeImg); // recreate entire volume image
		free_image(compositeImg);
    }
	Rasteron_Image* getSlice(unsigned d) const { return (d < depth) ? getFrame(data, d) : nullptr;}
	const Img_Base* extractVolImage() const { return &volumeImg; }

	unsigned getWidth() const { return width; }
	unsigned getHeight() const { return height; }
	unsigned getDepth() const { return depth; }
private:
	Img_Base volumeImg;
	Rasteron_Animation* data = nullptr; // underlying data
#endif
	const unsigned width, height, depth;
};

// Heightmap wrapper around Rasteron_Heightmap

struct Img_Heightmap : public Geo_Mesh { // wrapper around Rasteron_Heightmap
    Img_Heightmap() : Geo_Mesh(0){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Img_Heightmap(const Rasteron_Image* refImage)
    : Geo_Mesh(refImage->height * refImage->width) {
        heightmap = createHeightmap(refImage);
		genVertices(); genIndices();
    }

    ~Img_Heightmap(){ free_heightmap(heightmap); }
#endif
private:
    void genVertices() override {
#ifdef RASTERON_H
		for (unsigned r = 0; r < heightmap->height; r++)
			for (unsigned c = 0; c < heightmap->width; c++) {
				float x = (1.0f / heightmap->width) * c;
				float y = (1.0f / heightmap->height) * r;
				Vec3f pos = Vec3f({ x, y, (float)*(heightmap->data + (r * heightmap->width + c)) });
				Vec3f texcoord = Vec3f({ x, y, (float)*(heightmap->data + (r * heightmap->width + c)) });
				_vertices[(r * heightmap->width + c)] = Geo_Vertex(pos, texcoord);
			}
#endif
	}

    void genIndices() override { return; } // No indices by default

#ifdef RASTERON_H
    Rasteron_Heightmap* heightmap = NULL;
#endif
};