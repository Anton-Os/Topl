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
            _spriteSquares.back().second->modify(stretchTform, stretchFactorX, AXIS_Y);
            float stretchFactorY = getSpriteHeight(currentSprite) / getSpriteWidth(currentSprite); // get ratio between height and width
            stretchFactorY += (stretchFactorY < 1.0f) ? (1.0f - stretchFactorY) / 2 : -1 * ((stretchFactorY - 1.0f) / 2); // halve the stretch amount
            _spriteSquares.back().second->modify(stretchTform, stretchFactorY, AXIS_X);
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
            _spriteSquares.back().second->modify(stretchTform, stretchFactorX, AXIS_Y);
            float stretchFactorY = getSpriteHeight(currentSprite) / getSpriteWidth(currentSprite); // get ratio between height and width
            stretchFactorY += (stretchFactorY < 1.0f) ? (1.0f - stretchFactorY) / 2 : -1 * ((stretchFactorY - 1.0f) / 2); // halve the stretch amount
            _spriteSquares.back().second->modify(stretchTform, stretchFactorY, AXIS_X);
        }
    }
}

Geo_SpriteTable::~Geo_SpriteTable(){
        for(unsigned i = 0; i < _spriteCount; i++)
            delFileImage(_fileImages + i); // free all Rasteron specific file images
        if(_fileImages != nullptr) free(_fileImages); // free allocated space

        for(unsigned i = 0; i < _spriteCount; i++)
            deleteImg(*(_rastImages + i)); // free all Rasteron specific raw images
        if(_rastImages != nullptr) free(_rastImages); // free allocated space

        for(unsigned i = 0; i < _spriteCount; i++)
            deleteSprite(*(_rastSprites + i)); // free all Rasteron specific sprite objects
        if(_rastSprites != nullptr) free(_rastSprites); // free allocated space

		for(unsigned i = 0; i < _spriteCount; i++) // delete Topl Flat Shapes
			delete(*(_squares + i));
		if(_squares != nullptr) free(_squares);  // free allocated space
}

void Geo_SpriteTable::init(std::initializer_list<std::string> filePaths){
    _spriteCount = filePaths.size();

    // Rasteron specific object allocation
    _fileImages = (FileImage*)malloc(_spriteCount * sizeof(FileImage));
    _squares = (Geo_FlatSquare**)malloc(_spriteCount * sizeof(Geo_FlatSquare*));
    _rastImages = (Rasteron_Image**)malloc(_spriteCount * sizeof(Rasteron_Image*));
    _rastSprites = (Rasteron_Sprite**)malloc(_spriteCount * sizeof(Rasteron_Sprite*));

    unsigned offset = 0;
    for (std::initializer_list<std::string>::iterator currentFileName = filePaths.begin(); currentFileName < filePaths.end(); currentFileName++) {
		// std::replace((*(currentFileName)).begin(), (*(currentFileName)).end(), '/', '\\');
		std::string newFileName = *currentFileName;
		std::replace(newFileName.begin(), newFileName.end(), '/', '\\');

        loadFileImage(newFileName.c_str(), &(*(_fileImages + offset)));
        *(_rastImages + offset) = createImgBase(&(*(_fileImages + offset)));
        *(_rastSprites + offset) = createSprite(*(_rastImages + offset));
        
        offset++;
    }
}