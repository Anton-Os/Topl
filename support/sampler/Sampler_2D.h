#include "Sampler.h"

#ifdef RASTERON_H

struct Sampler_Color : public Sampler_2D {
    Sampler_Color(unsigned color) : Sampler_2D(){ // Sampler_2D("color", DEFAULT_SAMPLE_HEIGHT, DEFAULT_SAMPLE_WIDTH){
        cleanup();
        if(image != NULL) std::thread([this](unsigned c){ image = solidImgOp(getImgSize(), c); }, color);
        else image = solidImgOp(getImgSize(), color);
		tag = &image->name;
    }
};

struct Sampler_Gradient : public Sampler_2D {
    Sampler_Gradient(SIDE_Type side, unsigned color1, unsigned color2) : Sampler_2D(){ // Sampler_2D("color", DEFAULT_SAMPLE_HEIGHT, DEFAULT_SAMPLE_WIDTH){
        cleanup();
        if(image != NULL) std::thread([this](SIDE_Type s, unsigned c1, unsigned c2){ image = gradientImgOp(getImgSize(), s, c1, c2); }, side, color1, color2);
        else image = gradientImgOp(getImgSize(), side, color1, color2); 
		tag = &image->name;
    }
};

struct Sampler_Map : public Sampler_2D {
    Sampler_Map(coordCallback callback) : Sampler_2D(){
        cleanup();
		if(image != NULL) std::thread([this](coordCallback c){ image = mapImgOp(getImgSize(), c); }, callback);
		else image = mapImgOp(getImgSize(), callback);
		tag = &image->name;
    }
};

struct Sampler_Field : public Sampler_2D {
    Sampler_Field(fieldCallback callback) : Sampler_2D(){
        // TODO: Add points to table
        cleanup();
		if(image != NULL) std::thread([this](fieldCallback c, ColorPointTable* t){ image = fieldImgOp(getImgSize(), t, c); }, callback, &table);
		else image = fieldImgOp(getImgSize(), &table, callback);
		tag = &image->name;
    }
private:
    ColorPointTable table;
};

struct Sampler_Noise : public Sampler_2D {
    Sampler_Noise(ColorGrid colorGrid) : Sampler_2D(){
        grid = colorGrid;

        cleanup();
		if(image != NULL) std::thread([this](ColorGrid g){ image = noiseImgOp(getImgSize(), g); }, grid);
		else image = noiseImgOp(getImgSize(), grid);
		tag = &image->name;
    }
private:
    ColorGrid grid;
};

struct Sampler_Cellular : public Sampler_2D {
    Sampler_Cellular(ref_image_t refImg, nebrCallback8 callback){
        seedImg = &refImg;

        cleanup();
        if(image != NULL) std::thread([this](ref_image_t r, nebrCallback8 c){ image = cellwiseImgOp(r, c); }, refImg, callback);
		else image = cellwiseImgOp(refImg, callback);
		tag = &image->name;
    }
private:
    const ref_image_t* seedImg;
}; 

/* struct Sampler_Insert : public Sampler_2D {
    // TODO: Include body
}; */

struct Sampler_File : public Sampler_2D {
    Sampler_File(const std::string& filePath) : Sampler_2D(){
		cleanup();
		if(image != NULL) std::thread([this](const std::string& s){ image = loadImgOp(s.c_str()); }, filePath);
		else image = loadImgOp(filePath.c_str());
		tag = &image->name;
    }
};

struct Sampler_Text : public Sampler_2D {
    Sampler_Text(Rasteron_Text textObj) : Sampler_2D(){
		cleanup();
		if(image != NULL) std::thread([this](Rasteron_Text t){ image = textImgOp(&t, FONT_SIZE_MED); }, textObj);
		else image = textImgOp(&textObj, FONT_SIZE_MED); // TODO: Include padding
		tag = &image->name;
    }
};

#endif