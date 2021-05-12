#ifndef GEO_SPRITE_TABLE_H

#include <initializer_list>
#include <cstdlib>
#include <vector>

#include "Rasteron.h"

#include "Geometry.hpp"
#include "primitives/Geo_Flat.hpp"

typedef std::pair<const Rasteron_Sprite*, Geo_FlatSquare*> spriteSquare_pair;

static float getSpriteWidth(const Rasteron_Sprite* sprite) {
	return sprite->bounds.topRight_Pt[X_OFFSET] * 2;
	// return sprite->bounds.topRight_Pt[X_OFFSET]; // for testing
}
static float getSpriteHeight(const Rasteron_Sprite* sprite) {
	return sprite->bounds.topRight_Pt[Y_OFFSET] * 2;
	// return sprite->bounds.topRight_Pt[Y_OFFSET]; // for testing
}

static float stretchTransform(float input, double mod){ return input * mod; } // TODO: Make this code segment reusable

class Geo_SpriteTable {
public:
    Geo_SpriteTable(std::initializer_list<const char*> filePaths){
		init(filePaths);
		// Topl specific object loading
		for(unsigned offset = 0; offset < mSpriteCount; offset++){
			const Rasteron_Sprite* currentSprite = (const Rasteron_Sprite *)*(mRstnSprites + offset);
			*(mSquares + offset) = new Geo_FlatSquare((getSpriteWidth(currentSprite) + getSpriteHeight(currentSprite)) / 2);
			mSpriteSquares.push_back(std::make_pair(currentSprite, *(mSquares + offset)));
			if (getSpriteWidth(currentSprite) != getSpriteHeight(currentSprite)) { // render object stretching
				float stretchFactorX = getSpriteWidth(currentSprite) / getSpriteHeight(currentSprite); // get ratio between width and height
				stretchFactorX += (stretchFactorX < 1.0f) ? (1.0f - stretchFactorX) / 2 : -1 * ((stretchFactorX - 1.0f) / 2); // halve the stretch amount
				mSpriteSquares.back().second->modify(stretchTransform, stretchFactorX, AXIS_Y);
				float stretchFactorY = getSpriteHeight(currentSprite) / getSpriteWidth(currentSprite); // get ratio between height and width
				stretchFactorY += (stretchFactorY < 1.0f) ? (1.0f - stretchFactorY) / 2 : -1 * ((stretchFactorY - 1.0f) / 2); // halve the stretch amount
				mSpriteSquares.back().second->modify(stretchTransform, stretchFactorY, AXIS_X);
			}
		}
    }
	Geo_SpriteTable(std::initializer_list<const char*> filePaths, float scaleFactor){
		init(filePaths);
		// Topl specific object loading
		for(unsigned offset = 0; offset < mSpriteCount; offset++){
			const Rasteron_Sprite* currentSprite = (const Rasteron_Sprite *)*(mRstnSprites + offset);
			*(mSquares + offset) = new Geo_FlatSquare(((getSpriteWidth(currentSprite) + getSpriteHeight(currentSprite)) / 2) * scaleFactor);
			mSpriteSquares.push_back(std::make_pair(currentSprite, *(mSquares + offset)));
			if (getSpriteWidth(currentSprite) != getSpriteHeight(currentSprite)) { // render object stretching
				float stretchFactorX = getSpriteWidth(currentSprite) / getSpriteHeight(currentSprite); // get ratio between width and height
				stretchFactorX += (stretchFactorX < 1.0f) ? (1.0f - stretchFactorX) / 2 : -1 * ((stretchFactorX - 1.0f) / 2); // halve the stretch amount
				mSpriteSquares.back().second->modify(stretchTransform, stretchFactorX, AXIS_Y);
				float stretchFactorY = getSpriteHeight(currentSprite) / getSpriteWidth(currentSprite); // get ratio between height and width
				stretchFactorY += (stretchFactorY < 1.0f) ? (1.0f - stretchFactorY) / 2 : -1 * ((stretchFactorY - 1.0f) / 2); // halve the stretch amount
				mSpriteSquares.back().second->modify(stretchTransform, stretchFactorY, AXIS_X);
			}
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

		for(unsigned i = 0; i < mSpriteCount; i++) // delete Topl Flat Shapes
			delete(*(mSquares + i));
		if(mSquares != nullptr) free(mSquares);  // free allocated space
    }
	
	Rasteron_Sprite* getSprite(unsigned index) {
		assert(index < mSpriteSquares.size());
		return (Rasteron_Sprite*)mSpriteSquares.at(index).first;
	}

	Geo_FlatSquare* getSquare(unsigned index){
		assert(index < mSpriteSquares.size());
		return (Geo_FlatSquare*)mSpriteSquares.at(index).second;
	}

	unsigned getCount(){ return mSpriteSquares.size(); }
private:
	void init(std::initializer_list<const char*> filePaths){ // shared initialization code block
		mSpriteCount = filePaths.size();

        // Rasteron specific object allocation
        mFileImages = (FileImage*)malloc(mSpriteCount * sizeof(FileImage));
		mSquares = (Geo_FlatSquare**)malloc(mSpriteCount * sizeof(Geo_FlatSquare*));
        mRstnImages = (Rasteron_Image**)malloc(mSpriteCount * sizeof(Rasteron_Image*));
		mRstnSprites = (Rasteron_Sprite**)malloc(mSpriteCount * sizeof(Rasteron_Sprite*));

		unsigned offset = 0;
		for (std::initializer_list<const char*>::iterator currentFileName = filePaths.begin(); currentFileName < filePaths.end(); currentFileName++) {
			rstnLoadFromFile(*(currentFileName), &(*(mFileImages + offset)));
			*(mRstnImages + offset) = rstnCreate_ImgBase(&(*(mFileImages + offset)));
			*(mRstnSprites + offset) = rstnCreate_Sprite(*(mRstnImages + offset));
			
			offset++;
		}
	}

    unsigned mSpriteCount;
	std::vector<spriteSquare_pair> mSpriteSquares;
    FileImage* mFileImages = nullptr;
    Rasteron_Image** mRstnImages = nullptr;
    Rasteron_Sprite** mRstnSprites = nullptr;
	Geo_FlatSquare** mSquares = nullptr;
};

#define GEO_SPRITE_TABLE_H
#endif