#include "Sampler.h"

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

/* 

struct Sampler_Map : public Sampler_2D {
    // TODO: Include body
};

struct Sampler_Field : public Sampler_2D {
    // TODO: Include body
};

struct Sampler_Noise : public Sampler_2D {
    // TODO: Include body
};

struct Sampler_Cellular : public Sampler_2D {
    // TODO: Include body
}; 

struct Sampler_Insert : public Sampler_2D {
    // TODO: Include body
};

*/

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