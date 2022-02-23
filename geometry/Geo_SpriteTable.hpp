#ifndef GEO_SPRITE_TABLE_H

#include <initializer_list>
#include <cstdlib>
#include <vector>

#include "Rasteron.h"

#include "Geometry.hpp"
#include "primitives/Geo_Flat.hpp"

typedef std::pair<const Rasteron_Sprite*, Geo_FlatSquare*> spriteSquare_pair;

class Geo_SpriteTable {
public:
	Geo_SpriteTable(std::initializer_list<std::string> filePaths);
	Geo_SpriteTable(std::initializer_list<std::string> filePaths, float scaleFactor);
	~Geo_SpriteTable();
	
	Rasteron_Sprite* getSprite(unsigned index) {
		if(index < _spriteCount) return *(_sprites + index);
		else return nullptr; // error
	}

	Geo_FlatSquare* getSquare(unsigned index){
		if(index < _spriteCount) return *(_squares + index);
		else return nullptr;
	}

	unsigned getCount(){ return _spriteCount; }
private:
	void init(std::initializer_list<std::string> filePaths);

    unsigned _spriteCount;
    Rasteron_Image** _images = nullptr;
    Rasteron_Sprite** _sprites = nullptr;
	Geo_FlatSquare** _squares = nullptr;
};

#define GEO_SPRITE_TABLE_H
#endif