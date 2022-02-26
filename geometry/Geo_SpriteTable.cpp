#include "Geo_SpriteTable.hpp"

static float getSpriteWidth(const Rasteron_Sprite* sprite) {
	return sprite->bounds.topRight_point[X_OFFSET] * 2;
}
static float getSpriteHeight(const Rasteron_Sprite* sprite) {
	return sprite->bounds.topRight_point[Y_OFFSET] * 2;
}

static float stretchTform(float input, double mod){ return input * mod; }

static void resize(const Rasteron_Sprite* sprite, Geo_FlatSquare* square){
    float width = sprite->bounds.topRight_point[X_OFFSET] * 2;
    float height = sprite->bounds.topRight_point[Y_OFFSET] * 2;

    if (width != height) { // no stretch required for matchin width and height
        float stretchX = width / height; // get ratio between width and height
        stretchX += (stretchX < 1.0f) ? (1.0f - stretchX) / 2 : -1 * ((stretchX - 1.0f) / 2); // halve the stretch amount
        square->modify(stretchTform, stretchX, AXIS_X);
        
        float stretchY = height / width; // get ratio between height and width
        stretchY += (stretchY < 1.0f) ? (1.0f - stretchY) / 2 : -1 * ((stretchY - 1.0f) / 2); // halve the stretch amount
        square->modify(stretchTform, stretchY, AXIS_Y);
    }
}

Geo_SpriteTable::Geo_SpriteTable(std::initializer_list<std::string> filePaths){
    init(filePaths);

    for(unsigned offset = 0; offset < _spriteCount; offset++){
        const Rasteron_Sprite* currentSprite = (const Rasteron_Sprite *)*(_sprites + offset);
        *(_squares + offset) = new Geo_FlatSquare((getSpriteWidth(currentSprite) + getSpriteHeight(currentSprite)) / 2);
        Geo_FlatSquare* currentSquare = *(_squares + offset);
        
        resize(currentSprite, currentSquare);
    }
}

Geo_SpriteTable::Geo_SpriteTable(std::initializer_list<std::string> filePaths, float scale){
    init(filePaths);
    
    for(unsigned offset = 0; offset < _spriteCount; offset++){
        const Rasteron_Sprite* currentSprite = (const Rasteron_Sprite *)*(_sprites + offset);
        *(_squares + offset) = new Geo_FlatSquare(((getSpriteWidth(currentSprite) + getSpriteHeight(currentSprite)) / 2) * scale);
        Geo_FlatSquare* currentSquare = *(_squares + offset);
        
        resize(currentSprite, currentSquare);
    }
}

Geo_SpriteTable::~Geo_SpriteTable(){
        for(unsigned i = 0; i < _spriteCount; i++) deleteImg(*(_images + i));
        if(_images != nullptr) free(_images);

        for(unsigned i = 0; i < _spriteCount; i++) deleteSprite(*(_sprites + i));
        if(_sprites != nullptr) free(_sprites);

		for(unsigned i = 0; i < _spriteCount; i++) delete(*(_squares + i));
		if(_squares != nullptr) free(_squares);
}

void Geo_SpriteTable::init(std::initializer_list<std::string> filePaths){
    _spriteCount = filePaths.size();

    _images = (Rasteron_Image**)malloc(_spriteCount * sizeof(Rasteron_Image*));
    _sprites = (Rasteron_Sprite**)malloc(_spriteCount * sizeof(Rasteron_Sprite*));
    _squares = (Geo_FlatSquare**)malloc(_spriteCount * sizeof(Geo_FlatSquare*));

    unsigned offset = 0;
    for (std::initializer_list<std::string>::iterator currentFileName = filePaths.begin(); currentFileName < filePaths.end(); currentFileName++) {
		std::string newFileName = *currentFileName;
		std::replace(newFileName.begin(), newFileName.end(), '/', '\\'); // filepath fix

        *(_images + offset) = createImgRef(newFileName.c_str());
        *(_sprites + offset) = createSprite(*(_images + offset));
        
        offset++;
    }
}