#include "Geo_SpriteTable.hpp"

Geo_SpriteTable::Geo_SpriteTable(std::initializer_list<std::string> filePaths){
    init(filePaths);
    // Topl specific object loading
    for(unsigned offset = 0; offset < _spriteCount; offset++){
        const Rasteron_Sprite* currentSprite = (const Rasteron_Sprite *)*(_rastSprites + offset);
        *(_squares + offset) = new Geo_FlatSquare((getSpriteWidth(currentSprite) + getSpriteHeight(currentSprite)) / 2);
        _spriteSquares.push_back(std::make_pair(currentSprite, *(_squares + offset)));
        if (getSpriteWidth(currentSprite) != getSpriteHeight(currentSprite)) { // render object stretching
            float stretchFactorX = getSpriteWidth(currentSprite) / getSpriteHeight(currentSprite); // get ratio between width and height
            stretchFactorX += (stretchFactorX < 1.0f) ? (1.0f - stretchFactorX) / 2 : -1 * ((stretchFactorX - 1.0f) / 2); // halve the stretch amount
            _spriteSquares.back().second->modify(stretchTform, stretchFactorX, AXIS_X);
            float stretchFactorY = getSpriteHeight(currentSprite) / getSpriteWidth(currentSprite); // get ratio between height and width
            stretchFactorY += (stretchFactorY < 1.0f) ? (1.0f - stretchFactorY) / 2 : -1 * ((stretchFactorY - 1.0f) / 2); // halve the stretch amount
            _spriteSquares.back().second->modify(stretchTform, stretchFactorY, AXIS_Y);
        }
    }
}

Geo_SpriteTable::Geo_SpriteTable(std::initializer_list<std::string> filePaths, float scaleFactor){
    init(filePaths);
    // Topl specific object loading
    for(unsigned offset = 0; offset < _spriteCount; offset++){
        const Rasteron_Sprite* currentSprite = (const Rasteron_Sprite *)*(_rastSprites + offset);
        *(_squares + offset) = new Geo_FlatSquare(((getSpriteWidth(currentSprite) + getSpriteHeight(currentSprite)) / 2) * scaleFactor);
        _spriteSquares.push_back(std::make_pair(currentSprite, *(_squares + offset)));
        if (getSpriteWidth(currentSprite) != getSpriteHeight(currentSprite)) { // render object stretching
            float stretchFactorX = getSpriteWidth(currentSprite) / getSpriteHeight(currentSprite); // get ratio between width and height
            stretchFactorX += (stretchFactorX < 1.0f) ? (1.0f - stretchFactorX) / 2 : -1 * ((stretchFactorX - 1.0f) / 2); // halve the stretch amount
            _spriteSquares.back().second->modify(stretchTform, stretchFactorX, AXIS_X);
            float stretchFactorY = getSpriteHeight(currentSprite) / getSpriteWidth(currentSprite); // get ratio between height and width
            stretchFactorY += (stretchFactorY < 1.0f) ? (1.0f - stretchFactorY) / 2 : -1 * ((stretchFactorY - 1.0f) / 2); // halve the stretch amount
            _spriteSquares.back().second->modify(stretchTform, stretchFactorY, AXIS_Y);
        }
    }
}

Geo_SpriteTable::~Geo_SpriteTable(){
        for(unsigned i = 0; i < _spriteCount; i++)
            deleteImg(*(_images + i)); // free all Rasteron specific raw images
        if(_images != nullptr) free(_images); // free allocated space

        for(unsigned i = 0; i < _spriteCount; i++)
            deleteSprite(*(_rastSprites + i)); // free all Rasteron specific sprite objects
        if(_rastSprites != nullptr) free(_rastSprites); // free allocated space

		for(unsigned i = 0; i < _spriteCount; i++) // delete Topl Flat Shapes
			delete(*(_squares + i));
		if(_squares != nullptr) free(_squares);  // free allocated space
}

void Geo_SpriteTable::init(std::initializer_list<std::string> filePaths){
    _spriteCount = filePaths.size();

    _squares = (Geo_FlatSquare**)malloc(_spriteCount * sizeof(Geo_FlatSquare*));
    _images = (Rasteron_Image**)malloc(_spriteCount * sizeof(Rasteron_Image*));
    _rastSprites = (Rasteron_Sprite**)malloc(_spriteCount * sizeof(Rasteron_Sprite*));

    unsigned offset = 0;
    for (std::initializer_list<std::string>::iterator currentFileName = filePaths.begin(); currentFileName < filePaths.end(); currentFileName++) {
		std::string newFileName = *currentFileName;
		std::replace(newFileName.begin(), newFileName.end(), '/', '\\');

        // loadFileImage(newFileName.c_str(), &(*(_fileImages + offset)));
		*(_images + offset) = nullptr;
        *(_images + offset) = createImgRef(newFileName.c_str());
        *(_rastSprites + offset) = createSprite(*(_images + offset));
        
        offset++;
    }
}