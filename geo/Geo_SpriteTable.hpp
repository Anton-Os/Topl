#ifndef GEO_SPRITE_TABLE_H

#include <initializer_list>
#include <cstdlib>
#include <vector>

#include "Rasteron.h"

#include "Geometry.hpp"
#include "primitives/Geo_Flat.hpp"

typedef std::pair<const Rasteron_Sprite*, Geo_FlatSquare*> spriteSquare_pair;

static float getSpriteWidth(const Rasteron_Sprite* sprite) {
	return sprite->bounds.topRight_point[X_OFFSET] * 2;
	// return sprite->bounds.topRight_point[X_OFFSET]; // for testing
}
static float getSpriteHeight(const Rasteron_Sprite* sprite) {
	return sprite->bounds.topRight_point[Y_OFFSET] * 2;
	// return sprite->bounds.topRight_point[Y_OFFSET]; // for testing
}

static float stretchTform(float input, double mod){ return input * mod; } // TODO: Make this code segment reusable

class Geo_SpriteTable {
public:
	Geo_SpriteTable(std::initializer_list<std::string> filePaths);
	Geo_SpriteTable(std::initializer_list<std::string> filePaths, float scaleFactor);
	~Geo_SpriteTable();
	
	Rasteron_Sprite* getSprite(unsigned index) {
		assert(index < _spriteSquares.size());
		return (Rasteron_Sprite*)_spriteSquares.at(index).first;
	}

	Geo_FlatSquare* getSquare(unsigned index){
		assert(index < _spriteSquares.size());
		return (Geo_FlatSquare*)_spriteSquares.at(index).second;
	}

	unsigned getCount(){ return _spriteSquares.size(); }
private:
	void init(std::initializer_list<std::string> filePaths);

    unsigned _spriteCount;
	std::vector<spriteSquare_pair> _spriteSquares;
    FileImage* _fileImages = nullptr;
    Rasteron_Image** _rastImages = nullptr;
    Rasteron_Sprite** _rastSprites = nullptr;
	Geo_FlatSquare** _squares = nullptr;
};

#define GEO_SPRITE_TABLE_H
#endif