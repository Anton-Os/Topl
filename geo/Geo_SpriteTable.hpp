#ifndef GEO_SPRITE_TABLE_H

#include <initializer_list>
#include <cstdlib>
#include <vector>

#include "Rasteron.h"

#include "Geometry.hpp"
#include "primitives/Geo_Rect2D.hpp"
#include "primitives/Geo_Flat.hpp" // use to replace all instances of Rect2D

typedef std::pair<const Rasteron_Sprite*, Geo_Rect2D*> spriteRect_pair;
typedef std::pair<const Rasteron_Sprite*, Geo_FlatSquare*> spriteSquare_pair;

static float getSpriteWidth(const Rasteron_Sprite* sprite) { // TODO: Replace with something less crude
	return sprite->bounds.topRight_Pt[0] * 2;
}
static float getSpriteHeight(const Rasteron_Sprite* sprite) { // TODO: Replace with something less crude
	return sprite->bounds.topRight_Pt[1] * 2;
}

class Geo_SpriteTable {
public:
    Geo_SpriteTable(std::initializer_list<const char*> filePaths){
        mSpriteCount = filePaths.size(); // TODO: Uncomment this and use

        // Fill data
        mFileImages = (FileImage*)malloc(mSpriteCount * sizeof(FileImage));
        mRects = (Geo_Rect2D**)malloc(mSpriteCount * sizeof(Geo_Rect2D*)); // remove
		mSquares = (Geo_FlatSquare**)malloc(mSpriteCount * sizeof(Geo_FlatSquare*));
        mRstnImages = (Rasteron_Image**)malloc(mSpriteCount * sizeof(Rasteron_Image*));
        mRstnSprites = (Rasteron_Sprite**)malloc(mSpriteCount * sizeof(Rasteron_Sprite*));

		unsigned offset = 0;
		for (std::initializer_list<const char*>::iterator currentFileName = filePaths.begin(); currentFileName < filePaths.end(); currentFileName++) {
			// Rasteron specific object loading
			rstnLoadFromFile(*(currentFileName), &(*(mFileImages + offset)));
			*(mRstnImages + offset) = rstnCreate_ImgBase(&(*(mFileImages + offset)));
			*(mRstnSprites + offset) = rstnCreate_Sprite(*(mRstnImages + offset));

			// Topl specific object loading
			const Rasteron_Sprite* currentSprite = (const Rasteron_Sprite *)*(mRstnSprites + offset);
			*(mRects + offset) = new Geo_Rect2D(getSpriteWidth(currentSprite), getSpriteHeight(currentSprite)); // remove
			mSpriteBoxes.push_back(std::make_pair(*(mRstnSprites + offset), *(mRects + offset))); // remove
			*(mSquares + offset) = new Geo_FlatSquare((getSpriteWidth(currentSprite) + getSpriteHeight(currentSprite)) / 2);
			mSpriteSquares.push_back(std::make_pair(*(mRstnSprites + offset), *(mSquares + offset)));

			offset++;
		}
    }
	Geo_SpriteTable(std::initializer_list<const char*> filePaths, float scaleFactor){
        mSpriteCount = filePaths.size(); // TODO: Uncomment this and use

        // Fill data
        mFileImages = (FileImage*)malloc(mSpriteCount * sizeof(FileImage));
        mRects = (Geo_Rect2D**)malloc(mSpriteCount * sizeof(Geo_Rect2D*));
        mRstnImages = (Rasteron_Image**)malloc(mSpriteCount * sizeof(Rasteron_Image*));
        mRstnSprites = (Rasteron_Sprite**)malloc(mSpriteCount * sizeof(Rasteron_Sprite*));

		unsigned offset = 0;
		for (std::initializer_list<const char*>::iterator currentFileName = filePaths.begin(); currentFileName < filePaths.end(); currentFileName++) {
			rstnLoadFromFile(*(currentFileName), &(*(mFileImages + offset)));
			*(mRstnImages + offset) = rstnCreate_ImgBase(&(*(mFileImages + offset)));
			*(mRstnSprites + offset) = rstnCreate_Sprite(*(mRstnImages + offset));

			// Followed by geometric code
			*(mRects + offset) = new Geo_Rect2D(getSpriteWidth(*(mRstnSprites + offset)) * scaleFactor, getSpriteHeight(*(mRstnSprites + offset)) * scaleFactor);
			mSpriteBoxes.push_back(std::make_pair(*(mRstnSprites + offset), *(mRects + offset)));

			offset++;
		}
    }
    ~Geo_SpriteTable(){
        for(unsigned i = 0; i < mSpriteCount; i++)
            rstnDelFromFile(mFileImages + i); // free all Rasteron specific file images
        if(mFileImages != nullptr) free(mFileImages); // free allocated space

        for(unsigned i = 0; i < mSpriteCount; i++)
            rstnDel_Img(*(mRstnImages + i)); // free all Rasteron specific raw images
        if(mRstnImages != nullptr) free(mRstnImages); // free allocated space

        for(unsigned i = 0; i < mSpriteCount; i++)
            rstnDel_Sprite(*(mRstnSprites + i)); // free all Rasteron specific sprite objects
        if(mRstnSprites != nullptr) free(mRstnSprites); // free allocated space

		for (unsigned i = 0; i < mSpriteCount; i++) // delete Topl Rects // remove
			delete (*(mRects + i));
		if (mRects != nullptr) free(mRects); // free allocated space // remove

		for(unsigned i = 0; i < mSpriteCount; i++) // delete Topl Flat Shapes
			delete(*(mSquares + i));
		if(mSquares != nullptr) free(mSquares);  // free allocated space
    }
	
	Rasteron_Sprite* getSprite(unsigned index) {
		assert(index < mSpriteBoxes.size());
		return (Rasteron_Sprite*)mSpriteBoxes.at(index).first; // remove
		// return (Rasteron_Sprite*)mSpriteSquares.at(index).first;
	}

	Geo_Rect2D* getRect(unsigned index) {
		assert(index < mSpriteBoxes.size());
		return (Geo_Rect2D*)mSpriteBoxes.at(index).second;
	}

	Geo_FlatSquare* getSquare(unsigned index){
		assert(index < mSpriteSquares.size());
		return (Geo_FlatSquare*)mSpriteSquares.at(index).second;
	}

	// unsigned getCount() { return mSpriteBoxes.size(); } // remove
	unsigned getCount(){ return mSpriteSquares.size(); }
private:
	bool isInit = false;

    std::vector<spriteRect_pair> mSpriteBoxes; // remove
	std::vector<spriteSquare_pair> mSpriteSquares;

    unsigned mSpriteCount;

    FileImage* mFileImages = nullptr;
    Rasteron_Image** mRstnImages = nullptr;
    Rasteron_Sprite** mRstnSprites = nullptr;
	Geo_Rect2D** mRects = nullptr; // remove
	Geo_FlatSquare** mSquares = nullptr;
};

#define GEO_SPRITE_TABLE_H
#endif