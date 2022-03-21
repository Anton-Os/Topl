#include "support_def.h"

#include "Geometry.hpp"

#ifdef RASTERON_H // required library for loading images

struct Topl_Image { // wrapper around Rasteron_Image
    Topl_Image(){} // no image
    Topl_Image(unsigned color){ setColorImage(color); } // solid image
    Topl_Image(const std::string& filePath){ setFileImage(filePath); } // file image
    Topl_Image(FT_Library* freetypeLib, Rasteron_FormatText textObj){ setTextImage(freetypeLib, &textObj); } // text image
    Topl_Image(Rasteron_Image* refImage){ setImage(refImage); } // custom image
    ~Topl_Image(){
        if(image != NULL) deleteImg(image);
    }

    void setColorImage(unsigned color){
        if(image != NULL) deleteImg(image); // delte old image
        image = createImgBlank(256, 256, color);
    }
    void setFileImage(const std::string& filePath){
        if(image != NULL) deleteImg(image); // delte old image
        image = createImgRef(filePath.c_str());
    }
    void setTextImage(FT_Library* freetypeLib, Rasteron_FormatText* textObj){
        if(image != NULL) deleteImg(image); // delte old image
        /* Rasteron_Image* fontImage = bakeImgText(freetypeLib, textObj);
        image = createImgFlip(fontImage, FLIP_Upside); // flip required to fix rendering
        deleteImg(fontImage); */
		image = bakeImgText(freetypeLib, textObj);
    }
    void setImage(Rasteron_Image* refImage){
        if(image != NULL) deleteImg(image); // delte old image
        image = refImage;
    }
    Rasteron_Image* getImage(){ return image; }
private:
    Rasteron_Image* image = NULL; // underlying data
};

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
    MATERIAL_Base = 0,
    // TODO: Add more properties
};

struct Topl_Material { // Material of static images, wrapper around Rasteron_Frames
    Topl_Material(std::string prefix, unsigned height, unsigned width){
	 	data = allocNewAnim(prefix.c_str(), height, width, MAX_MATERIAL_PROPERTIES);
	}
	~Topl_Material(){ deleteAnim(data); }
	
	void addLayer(const Rasteron_Image *const refImg, MATERIAL_Property property){
        addFrameData(data, refImg, property);
    }
	Rasteron_Image* getLayer(MATERIAL_Property property){ return getFrame(data, property); }

private:
	Rasteron_Animation* data; // underlying data
};

struct Topl_Heightmap : public Geo_RenderObj { // wrapper around Rasteron_Heightmap
    Topl_Heightmap(const Rasteron_Image* refImage)
    : Geo_RenderObj(refImage->height * refImage->width) {
        heightmap = createHeightmap(refImage);
    }

    ~Topl_Heightmap(){ deleteHeightmap(heightmap); }
private:
    void genPos(Eigen::Vector3f* data) override { return; } // Implement here
    void genNormals(Eigen::Vector3f* data) override { return; } // Implement here
    void genTexCoords(Eigen::Vector2f* data) override { return; } // Implement here
    void genIndices(unsigned* data) override { return; } // Implement here

    Rasteron_Heightmap* heightmap = NULL;
};

#endif