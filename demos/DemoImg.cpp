#include <iostream>

#include "Rasteron.h"

int main(void){

	Image img2;
	loadImage_TIFF("C:\\AntonDocs\\Design\\PurpleCult.tif", &img2);
	// loadImage_BMP("C:\\AntonDocs\\Design\\PurpleCult3.bmp", &img2);

	Rasteron_Image* rImage = rstnCreate_ImgBase(&img2);
	Rasteron_Palette* rPalette = rstnCreate_Palette(rImage);
	Rasteron_Outline* rOutline = rstnCreate_Outline(rImage);
	Rasteron_Sprite* rSprite = rstnCreate_Sprite(rImage);

	rstnDel_Img(rImage);
	rstnDel_Palette(rPalette);
	rstnDel_Outline(rOutline);
	rstnDel_Sprite(rSprite);

	delImage_TIFF(&img2);

    return 0;
}