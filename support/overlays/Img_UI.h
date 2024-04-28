#include "support_def.h"

#ifdef RASTERON_H

struct Img_UI {
	Img_UI(enum MENU_Size s){ size = s; }

	~Img_UI(){ RASTERON_QUEUE_DEALLOC(queue); }

	void setState(unsigned short index){
		queue->index = index % queue->frameCount;
		stateImg.setImage(queue_getImg(queue, index % queue->frameCount));
	}
	unsigned short getState(){ return queue->index; }

	Img_Base stateImg;
protected:
	MENU_Size size;
	Rasteron_Queue* queue;
};

// Implementation of UI elements

struct Img_Button : public Img_UI {
	Img_Button(enum MENU_Size size) : Img_UI(size){ 
		queue = loadUI_checkBtn(size); 
		Img_UI::setState(0);
	}

	Img_Button(enum MENU_Size size, char* iconName) : Img_UI(size){ 
		queue = loadUI_iconBtn(size, iconName);
		Img_UI::setState(0);
	}

	void setState(enum MENU_ItemState s){ Img_UI::setState((unsigned short)s); }
};

struct Img_Label : public Img_UI {
	Img_Label(enum MENU_Size size, Rasteron_Text textObj) : Img_UI(size){
		Rasteron_Text textObjs[4] = {
			textObj,
			{ textObj.fontFile, textObj.text, UI_COLOR_ON, textObj.fgColor },
			{ textObj.fontFile, textObj.text, UI_COLOR_OFF, textObj.fgColor },
			{ textObj.fontFile, textObj.text, UI_COLOR_DEFAULT, textObj.fgColor }
		};
		queue = RASTERON_QUEUE_ALLOC("label", createImgSize(10, 100), 4); // TODO: dynamically compute size
		for(unsigned short t = 0; t < 4; t++){
			Rasteron_Image* textImg = textImgOp(&textObjs[t], FONT_SIZE_MED);
			queue_addImg(queue, textImg, t);
			RASTERON_DEALLOC(textImg);
		}
		Img_UI::setState(0);
	}

	// TODO: Implement preset labels
	// TODO: Implement textField labels

	void setState(enum MENU_ItemState s){ Img_UI::setState((unsigned short)s); }
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