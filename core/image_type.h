#include <string>

#include "support_def.h"

#include "Geometry.hpp"

// Base Image wrapper around Rasteron_Image

struct Img_Base {
    Img_Base(){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Img_Base(unsigned color){ setColorImage(color); } // Solid Constructor
    Img_Base(const std::string& filePath){ setFileImage(filePath); } // File Constructor
    Img_Base(Rasteron_Text textObj){ 
        setTextImage( &textObj); // Text Constructor
    }
    Img_Base(Rasteron_Image* refImage){ setImage(refImage); } // Custom Constructor
    ~Img_Base(){
        if(image != NULL) deleteImg(image);
    }

    void setColorImage(unsigned color){
        if(image != NULL) deleteImg(image); // delete old image
		image = createSolidImg({ 256, 256 }, color);
    }
    void setFileImage(const std::string& filePath){
        if(image != NULL) deleteImg(image); // delete old image
		char newFilePath[1024];
		strcpy(&newFilePath[0], filePath.c_str());
#ifdef _WIN32
		replaceFwdSlash(&newFilePath[0]);
#endif
        image = createRefImg(&newFilePath[0]);
    }
    void setTextImage(Rasteron_Text* textObj){
        if(image != NULL) deleteImg(image); // delete old image
		image = bakeTextI(textObj);
    }
    void setImage(Rasteron_Image* refImage){
        if(image != NULL) deleteImg(image); // delete old image
        image = createCopyImg(refImage);
    }
    Rasteron_Image* getImage(){ return image; }
private:
    Rasteron_Image* image = NULL; // underlying data
#endif
};

// Frames of images implementing Rasteron_Animation

struct Img_Frames {
    Img_Frames(){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
	Img_Frames(std::string prefix, unsigned height, unsigned width, unsigned short frameCount){
		data = allocNewAnim(prefix.c_str(), { height, width }, frameCount);
	}
	~Img_Frames(){ deleteAnim(data); }
	
	void addNewFrame(const Rasteron_Image *const refImg){
        if(frameIndex == data->frameCount){
		    frameIndex = 0; 
            replay++;
        }
        addFrame(data, refImg, frameIndex);
        frameIndex++;
    }
	Rasteron_Image* getFrameAt(unsigned index) const { return getFrame(data, index); }

private:
    unsigned short frameIndex = 0; // keeps track of current frame for draw
    unsigned short replay = 0; // increments as scene replays itself

    Rasteron_Animation* data; // underlying data
#endif
};

// Material layers of images implementing Rasteron_Animation

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
    Img_Material(){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Img_Material(std::string prefix, unsigned height, unsigned width){
		data = allocNewAnim(prefix.c_str(), { height, width }, MAX_MATERIAL_PROPERTIES);
	}
	~Img_Material(){ deleteAnim(data); }
	
	void addNewLayer(const Rasteron_Image *const refImg, MATERIAL_Property property){
        addFrame(data, refImg, property);
    }
	Rasteron_Image* getLayer(MATERIAL_Property property) const { return getFrame(data, property); }

private:
	Rasteron_Animation* data; // underlying data
#endif
};

// Heightmap wrapper around Rasteron_Heightmap

struct Img_Heightmap : public Geo_RenderObj { // wrapper around Rasteron_Heightmap
    Img_Heightmap(){} // Empty Constructor
#ifdef RASTERON_H // required library for loading images
    Img_Heightmap(const Rasteron_Image* refImage)
    : Geo_RenderObj(refImage->height * refImage->width) {
        heightmap = createHeightmap(refImage);
		fillRenderObj();
    }

    ~Img_Heightmap(){ deleteHeightmap(heightmap); }
private:
    void genPos(Vec3f* data) override {
		for (unsigned p = 0; p < heightmap->width * heightmap->height; p++)
			*(data + p) = Vec3f({ 0.0f, (float)*(heightmap->data + p), 0.0f });
	}
    void genNormals(Vec3f* data) override { 
		for (unsigned p = 0; p < heightmap->width * heightmap->height; p++)
			*(data + p) = Vec3f({ 0.0f, (float)*(heightmap->data + p), 0.0f });
	} 
    void genTexCoords(Vec2f* data) override {
		float xInc = 1.0f / heightmap->width;
		float yInc = 1.0f / heightmap->height;

		for (unsigned r = 0; r < heightmap->height; r++)
			for (unsigned c = 0; c < heightmap->width; c++)
				*(data + (r * heightmap->width) + c) = Vec2f({ xInc * c, yInc * r });
	}
    void genIndices(unsigned* data) override { return; } // No indices by default

    Rasteron_Heightmap* heightmap = NULL;
#endif
};