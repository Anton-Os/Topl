#include "support_def.h"

struct Sampler_UI {
#ifdef RASTERON_H
	Sampler_UI(enum MENU_Size s){ size = s; }

	~Sampler_UI(){ RASTERON_QUEUE_DEALLOC(queue); }

	virtual void setState(unsigned short index){
		assert(queue != nullptr);
		queue->index = index % queue->frameCount;
		stateImg.setImage(queue_getImg(queue, index % queue->frameCount));
	}
	std::string getName(){ return std::string(queue->prefix); }
	unsigned short getState(){ return queue->index; }

    Sampler_2D stateImg;
protected:
	MENU_Size size;
	Rasteron_Queue* queue = nullptr;
#endif
};

// Implementation of UI elements

struct Sampler_Button : public Sampler_UI {
#ifdef RASTERON_H
	Sampler_Button(enum MENU_Size size) : Sampler_UI(size){
		queue = loadUI_checkBtn(size); 
		Sampler_UI::setState(MENU_None);
	}

	Sampler_Button(enum MENU_Size size, char* iconName) : Sampler_UI(size){ 
		queue = loadUI_iconBtn(size, iconName);
		Sampler_UI::setState(MENU_None);
	}
#endif
};

struct Sampler_Label : public Sampler_UI {
#ifdef RASTERON_H
	Sampler_Label(enum MENU_Size size, Rasteron_Text textObj) : Sampler_UI(size){
		queue = RASTERON_QUEUE_ALLOC("label", RASTERON_SIZE(100, 100), 4);
		setText(textObj);
		Sampler_UI::setState(MENU_None);
	}

	void setText(Rasteron_Text textObj){ // TODO: Make this operation threaded?
		if(textObj.fontFile != "" && textObj.text != ""){
			textObjs[0] = textObj;
			textObjs[1] = { textObj.fontFile, textObj.text, UI_COLOR_ON, textObj.fgColor };
			textObjs[2] = { textObj.fontFile, textObj.text, UI_COLOR_OFF, textObj.fgColor };
			textObjs[3] = { textObj.fontFile, textObj.text, UI_COLOR_DEFAULT, textObj.fgColor };

			Rasteron_Image* textImg = textPadImgOp(&textObj, FONT_SIZE_MED, paddings);
			for(unsigned short t = 0; t < 4; t++){
				Rasteron_Image* tempImg = copyImgOp(textImg);
				for(unsigned p = 0; p < textImg->width * textImg->height && t != MENU_None; p++){
					if(t == MENU_Pre && *(tempImg->data + p) != textObj.fgColor) *(tempImg->data + p) = textObjs[t].bkColor;
					else {
						double x = (1.0 / (double)tempImg->width) * (p % tempImg->width);
						double y = (1.0 / (double)tempImg->height) * (p / tempImg->width);
						if(x < 0.035 || x > 0.965 || y < 0.035 || y > 0.965) *(tempImg->data + p) = textObjs[t].bkColor;
					}
				}
				queue_addImg(queue, tempImg, t);
				RASTERON_DEALLOC(tempImg);
			}
			RASTERON_DEALLOC(textImg);

			Sampler_UI::setState(MENU_None);
		}
	}

	void setText(Rasteron_Text textObj, unsigned pads[4]){ // with updated paddings
		for(unsigned p = 0; p < 4; p++) paddings[p] = pads[p];
		setText(textObj);
	}

	// void setMessage(Rasteron_Message message){} // TODO: Enable multi-line functionality
private:
	Rasteron_Text textObjs[4]; // Default, On, Off, and Preselct States

	unsigned short paddings[4] = { 0, 0, 0, 0}; // Left, Right, Top, and Bottom paddings
#endif
};

struct Sampler_Item : public Sampler_UI {
#ifdef RASTERON_H
	Sampler_Item(enum MENU_Size size, SIDE_Type side, Rasteron_Text text, Rasteron_Image* image, unsigned bkColor) : Sampler_UI(size){ 
		Rasteron_Image* textImg = textImgOp(&text, (unsigned)size); // TODO: Get from menu size
		Rasteron_Image* backgroundImg;
		Rasteron_Image* insertImgs[2];
		if(side != SIDE_Left && side != SIDE_Right){
			queue = RASTERON_QUEUE_ALLOC("item", RASTERON_SIZE((unsigned)size + textImg->height + image->height, (unsigned)size + image->width ), 4);
			backgroundImg = solidImgOp({ (unsigned)size + textImg->height + image->height, (unsigned)size + image->width }, bkColor);
			insertImgs[0] = insertImgOp(image, backgroundImg, 0.0, 0.0);
			insertImgs[1] = insertImgOp(textImg, insertImgs[0], 0.0, (side == SIDE_Top)? 0.9 : -0.9);
		} else {
			queue = RASTERON_QUEUE_ALLOC("item", RASTERON_SIZE((unsigned)size + image->height, (unsigned)size + textImg->width + image->width ), 4);
			backgroundImg = solidImgOp({ (unsigned)size + image->height, (unsigned)size + textImg->width + image->width }, bkColor);
			insertImgs[0] = insertImgOp(image, backgroundImg, (side == SIDE_Left)? 0.9 : -0.9, 0.0);
			insertImgs[1] = insertImgOp(textImg, insertImgs[0], (side == SIDE_Left)? -0.9 : 0.9, 0.0);
		}
		for(unsigned b = 0; b < 4; b++) queue_addImg(queue, insertImgs[1], b);
		// TODO: Modify images to correspond to 4 menu states
		Sampler_UI::setState(MENU_None);

		RASTERON_DEALLOC(backgroundImg);
		RASTERON_DEALLOC(textImg);
		RASTERON_DEALLOC(insertImgs[0]); RASTERON_DEALLOC(insertImgs[1]);
	}
#endif
};


struct Sampler_Dial : public Sampler_UI {
#ifdef RASTERON_H
	Sampler_Dial(enum MENU_Size size, unsigned short count) : Sampler_UI(size){ 
		queue = loadUI_dial(size, count); 
		Sampler_UI::setState(0);
	}

	void setState(double x, double y){
		unsigned short index = 0;
		double minDist = 1.0;
		for(unsigned f = 0; f < queue->frameCount; f++){
			double indicX = 0.5 + sin(f * ((3.141592653 * 2) / queue->frameCount)); // x calculation
            double indicY = 0.5 + cos(f * ((3.141592653 * 2) / queue->frameCount)); // y calculation
			if(sqrt((fabs(indicX - x) * fabs(indicX - x)) + (fabs(indicY - y) * fabs(indicY - y))) < minDist){
				index = f;
				minDist = sqrt((fabs(indicX - x) * fabs(indicX - x)) + (fabs(indicY - y) * fabs(indicY - y)));
			}
		}
		Sampler_UI::setState(index);
	}
#endif
};

struct Sampler_Slider : public Sampler_UI {
#ifdef RASTERON_H
	Sampler_Slider(enum MENU_Size size, unsigned short count) : Sampler_UI(size){ 
		queue = loadUI_slider(size, count); 
		Sampler_UI::setState(0);
	}

	void setState(double x){
		unsigned short index = 0;
		double minDist = 1.0;
		for(unsigned f = 0; f < queue->frameCount; f++){
			double sliderX = ((1.0 / ((double)queue->frameCount - 1)) * f);
			if(sqrt(fabs(sliderX - x) * fabs(sliderX - x)) < minDist){
				index = f;
				minDist = sqrt(fabs(sliderX - x) * fabs(sliderX - x));
			}
		}
		Sampler_UI::setState(index);
	}
#endif
};
