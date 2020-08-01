#include <initializer_list>
#include <cassert> // Might meed error handling library
#include <utility>
#include <cstdlib>
#include <vector>

#include "Rasteron.h"

#include "Geometry.hpp"

typedef std::pair<const Rasteron_Sprite*, Geo_Rect2D*> spriteRect_pair;

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
        mRects = (Geo_Rect2D**)malloc(mSpriteCount * sizeof(Geo_Rect2D*));
        mRstnImages = (Rasteron_Image**)malloc(mSpriteCount * sizeof(Rasteron_Image*));
        mRstnSprites = (Rasteron_Sprite**)malloc(mSpriteCount * sizeof(Rasteron_Sprite*));

		unsigned dataOffset = 0;
		for (std::initializer_list<const char*>::iterator currentFileName = filePaths.begin(); currentFileName < filePaths.end(); currentFileName++) {
			rstnLoadFromFile(*(currentFileName), &(*(mFileImages + dataOffset)));
			*(mRstnImages + dataOffset) = rstnCreate_ImgBase(&(*(mFileImages + dataOffset)));
			*(mRstnSprites + dataOffset) = rstnCreate_Sprite(*(mRstnImages + dataOffset));

			// Followed by geometric code
			*(mRects + dataOffset) = new Geo_Rect2D(getSpriteWidth(*(mRstnSprites + dataOffset)), getSpriteHeight(*(mRstnSprites + dataOffset)));
			mSpriteBoxes.push_back(std::make_pair(*(mRstnSprites + dataOffset), *(mRects + dataOffset)));

			dataOffset++;
		}
    }
	Geo_SpriteTable(std::initializer_list<const char*> filePaths, float scaleFactor){
        mSpriteCount = filePaths.size(); // TODO: Uncomment this and use

        // Fill data
        mFileImages = (FileImage*)malloc(mSpriteCount * sizeof(FileImage));
        mRects = (Geo_Rect2D**)malloc(mSpriteCount * sizeof(Geo_Rect2D*));
        mRstnImages = (Rasteron_Image**)malloc(mSpriteCount * sizeof(Rasteron_Image*));
        mRstnSprites = (Rasteron_Sprite**)malloc(mSpriteCount * sizeof(Rasteron_Sprite*));

		unsigned dataOffset = 0;
		for (std::initializer_list<const char*>::iterator currentFileName = filePaths.begin(); currentFileName < filePaths.end(); currentFileName++) {
			rstnLoadFromFile(*(currentFileName), &(*(mFileImages + dataOffset)));
			*(mRstnImages + dataOffset) = rstnCreate_ImgBase(&(*(mFileImages + dataOffset)));
			*(mRstnSprites + dataOffset) = rstnCreate_Sprite(*(mRstnImages + dataOffset));

			// Followed by geometric code
			*(mRects + dataOffset) = new Geo_Rect2D(getSpriteWidth(*(mRstnSprites + dataOffset)) * scaleFactor, getSpriteHeight(*(mRstnSprites + dataOffset)) * scaleFactor);
			mSpriteBoxes.push_back(std::make_pair(*(mRstnSprites + dataOffset), *(mRects + dataOffset)));

			dataOffset++;
		}
    }
    ~Geo_SpriteTable(){
        for(unsigned i = 0; i < mSpriteCount; i++)
            rstnDelFromFile(mFileImages + i); // Get rid of all file images, POINTER PROVIDED!!!
        if(mFileImages != nullptr) free(mFileImages); // Free file image space

        for(unsigned i = 0; i < mSpriteCount; i++)
            rstnDel_Img(*(mRstnImages + i));
        if(mRstnImages != nullptr) free(mRstnImages);

        for(unsigned i = 0; i < mSpriteCount; i++)
            rstnDel_Sprite(*(mRstnSprites + i));
        if(mRstnSprites != nullptr) free(mRstnSprites);

		for (unsigned i = 0; i < mSpriteCount; i++) // new and delete keywords for Topl specific objects
			delete (*(mRects + i));
		if (mRects != nullptr) free(mRects);
    }
	
	Rasteron_Sprite* getSprite(unsigned index) {
		assert(index < mSpriteBoxes.size());
		return (Rasteron_Sprite*)mSpriteBoxes.at(index).first;
	}

	Geo_Rect2D* getRect(unsigned index) {
		assert(index < mSpriteBoxes.size());
		return (Geo_Rect2D*)mSpriteBoxes.at(index).second;
	}

	/* Geo_Rect2D* getRect(unsigned index, float scaling) {
		assert(index < mSpriteBoxes.size());
		return (Geo_Rect2D*)mSpriteBoxes.at(index).second;
	} */

	unsigned getCount() { return mSpriteBoxes.size(); }
private:
	bool isInit = false;

    std::vector<spriteRect_pair> mSpriteBoxes;

    unsigned mSpriteCount;

    FileImage* mFileImages = nullptr;
    Rasteron_Image** mRstnImages = nullptr;
    Rasteron_Sprite** mRstnSprites = nullptr;
	Geo_Rect2D** mRects = nullptr;
};