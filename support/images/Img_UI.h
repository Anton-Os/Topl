#include "support_def.h"

#ifdef RASTERON_H

struct Img_UI {
	Img_UI(enum MENU_Size s){ size = s; }

	~Img_UI(){ RASTERON_QUEUE_DEALLOC(queue); }

	virtual void setState(unsigned short index){
		assert(queue != nullptr);
		queue->index = index % queue->frameCount;
		stateImg.setImage(queue_getImg(queue, index % queue->frameCount));
	}
	std::string getName(){ return std::string(queue->prefix); }
	unsigned short getState(){ return queue->index; }

	Img_Base stateImg;
protected:
	MENU_Size size;
	Rasteron_Queue* queue = nullptr;
};

// Implementation of UI elements

struct Img_Button : public Img_UI {
	Img_Button(enum MENU_Size size) : Img_UI(size){
		queue = loadUI_checkBtn(size); 
		Img_UI::setState(MENU_None);
	}

	Img_Button(enum MENU_Size size, char* iconName) : Img_UI(size){ 
		queue = loadUI_iconBtn(size, iconName);
		Img_UI::setState(MENU_None);
	}
};

struct Img_Label : public Img_UI {
	Img_Label(enum MENU_Size size, Rasteron_Text textObj) : Img_UI(size){
		queue = RASTERON_QUEUE_ALLOC("label", RASTERON_SIZE(100, 100), 4);
		setText(textObj);
		Img_UI::setState(MENU_None);
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

			Img_UI::setState(MENU_None);
		}
	}

	void setPadding(unsigned short l, unsigned short r, unsigned short t, unsigned short b){
		// paddings[0] = l; paddings[1] = r;paddings[2] = t;paddings[3] = b;
		// setText(textObjs[0]);
	}
	
	void setString(const std::string& str){
		// textObjs[0].text = str.c_str();
		// setText(textObjs[0]);
	}

	// void setMessage(Rasteron_Message message){} // TODO: Enable multi-line functionality
private:
	Rasteron_Text textObjs[4]; // Default, On, Off, and Preselct States

	unsigned short paddings[4] = { 0, 0, 0, 0}; // Left, Right, Top, and Bottom paddings
};

struct Img_Item : public Img_UI {
	Img_Item(enum MENU_Size size, SIDE_Type side, Rasteron_Text text, Rasteron_Image* image, unsigned bkColor) : Img_UI(size){ 
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
		Img_UI::setState(MENU_None);

		RASTERON_DEALLOC(backgroundImg);
		RASTERON_DEALLOC(textImg);
		RASTERON_DEALLOC(insertImgs[0]); RASTERON_DEALLOC(insertImgs[1]);
	}
};


struct Img_Dial : public Img_UI {
	Img_Dial(enum MENU_Size size, unsigned short count) : Img_UI(size){ 
		queue = loadUI_dial(size, count); 
		Img_UI::setState(0);
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
		Img_UI::setState(index);
	}
};

struct Img_Slider : public Img_UI {
	Img_Slider(enum MENU_Size size, unsigned short count) : Img_UI(size){ 
		queue = loadUI_slider(size, count); 
		Img_UI::setState(0);
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
		Img_UI::setState(index);
	}
};

#endif