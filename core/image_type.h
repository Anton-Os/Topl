#include <string>

#include "support_def.h"

#include "Geometry.hpp"

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
			deleteImg(image);
			image = NULL; // reset
		}
	}

    Rasteron_Image* image = NULL; // underlying data
#endif
};

// Frames based on a sequence

struct Img_Frames {
    Img_Frames(){} // Empty Constructor
#ifdef RASTERON_H
	Img_Frames(std::string prefix, unsigned short frameCount){
		data = allocNewAnim(prefix.c_str(), frameCount);
	}
	~Img_Frames(){ deleteAnim(data); }
	
	void appendFrame(ref_image_t refImg){
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
#endif
};

// Material based on layers

#define MAX_MATERIAL_PROPERTIES 6 // matches MATERIAL_Property enumeration

enum MATERIAL_Property {
	MATERIAL_Albedo = 0,
	MATERIAL_Normals = 1,
	MATERIAL_Height = 2,
	MATERIAL_Metal = 3,
	MATERIAL_Roughness = 4,
	MATERIAL_Occlusion = 5
};

struct Img_Material {
	Img_Material() : width(IMG_SIDE_LEN), height(IMG_SIDE_LEN) {} // Empty Constructor
#ifdef RASTERON_H
	Img_Material(std::string prefix, unsigned w, unsigned h) : width(w), height(h) {
		data = allocNewAnim(prefix.c_str(), MAX_MATERIAL_PROPERTIES);
	}
	~Img_Material() { deleteAnim(data); }

	void addLayer(ref_image_t refImg, MATERIAL_Property property) {
		if (refImg->height == height && refImg->width == width)
			addFrameData(data, refImg, property);
		else return; // error
		materialImg = Img_Base(createCompositeImg(data));
	}
	Rasteron_Image* getLayer(MATERIAL_Property property) const { return getFrame(data, property); }
	const Img_Base* extractMatImg() const { return &materialImg; }

private:
	Img_Base materialImg;
	Rasteron_Animation* data = nullptr; // underlying data
#endif
	const unsigned width, height;
};

// Volume based on slices

struct Img_Volume { 
    Img_Volume() : width(IMG_SIDE_LEN), height(IMG_SIDE_LEN), depth(IMG_SIDE_LEN) {} // Empty Constructor
#ifdef RASTERON_H
    Img_Volume(std::string prefix, unsigned w, unsigned h,unsigned z) : width(w), height(h), depth(z) {
		data = allocNewAnim(prefix.c_str(), depth);
	}
	~Img_Volume(){ deleteAnim(data); }
	
	void addSlice(ref_image_t refImg, unsigned d){
		if (refImg->height == height && refImg->width == width)
			addFrameData(data, refImg, d);
		else return; // error
		volumeImg = Img_Base(createCompositeImg(data)); // recreate material
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

struct Img_Heightmap : public Geo_Renderable { // wrapper around Rasteron_Heightmap
    Img_Heightmap() : Geo_Renderable(0){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Img_Heightmap(const Rasteron_Image* refImage)
    : Geo_Renderable(refImage->height * refImage->width) {
        heightmap = createHeightmap(refImage);
		genVertices(); genIndices();
    }

    ~Img_Heightmap(){ deleteHeightmap(heightmap); }
#endif
private:
    void genVertices() override {
#ifdef RASTERON_H
		for (unsigned r = 0; r < heightmap->height; r++)
			for (unsigned c = 0; c < heightmap->width; c++) {
				Vec3f pos = Vec3f({ 0.0f, (float)*(heightmap->data + (r * heightmap->width + c)), 0.0f });
				Vec2f texcoord = Vec2f({ (1.0f / heightmap->width) * c, (1.0f / heightmap->height) * r });
				_vertices[(r * heightmap->width + c)] = Geo_Vertex(pos, texcoord);
			}
#endif
	}

    void genIndices() override { return; } // No indices by default

#ifdef RASTERON_H
    Rasteron_Heightmap* heightmap = NULL;
#endif
};