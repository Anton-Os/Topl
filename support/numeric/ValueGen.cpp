#include "ValueGen.hpp"


// Numeric Operations

unsigned genColorID(unsigned renderID) {
	// unsigned short colorInc = (renderID / COLOR_ID_TYPES) + 1;
	unsigned short colorInc = ((renderID / COLOR_ID_TYPES) + 1) * 70; // testing for emphasis

	switch (renderID % COLOR_ID_TYPES) {
	case 0: return 0xFFFFFFFF - ((colorInc << 16) + (colorInc << 8) + colorInc);
	case 1: return 0xFFFFFFFF - ((colorInc << 8) + colorInc);
	case 2: return 0xFFFFFFFF - ((colorInc << 16) + colorInc);
	case 3: return 0xFFFFFFFF - ((colorInc << 16) + (colorInc << 8));
	case 4: return 0xFFFFFFFF - colorInc;
	case 5: return 0xFFFFFFFF - (colorInc << 8);
	case 6: return 0xFFFFFFFF - (colorInc << 16);
	}
}

unsigned genRandColor() {
	uint8_t redBit = rand() % 255;
	uint8_t greenBit = rand() % 255;
	uint8_t blueBit = rand() % 255;
	return (uint32_t)((0xFF << 24) + (redBit << 16) + (greenBit << 8) + blueBit);
}

// Data Operations

SpatialBounds3D::SpatialBounds3D(float scaleFactor) {
	left *= scaleFactor;
	right *= scaleFactor;
	bottom *= scaleFactor;
	top *= scaleFactor;
	nearPlane *= scaleFactor;
	farPlane *= scaleFactor;
}

SpatialBounds3D::SpatialBounds3D(float l, float r, float b, float t, float n, float f) {
	left = l;
	right = r;
	bottom = b;
	top = t;
	nearPlane = n;
	farPlane = f;
}


void assignDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes){
    targetBytes->clear();
    targetBytes->resize(dataSize);
    
    for(unsigned d = 0; d < dataSize; d++)
        if(data_ptr + d != nullptr) targetBytes->at(d) = *(data_ptr + d);
}

void alignDataToBytes(const uint8_t* data_ptr, size_t dataSize, size_t paddingSize, std::vector<uint8_t>* targetBytes) {
	for (unsigned d = 0; d < dataSize + paddingSize; d++)
		(d < dataSize && data_ptr + d != nullptr)
		? targetBytes->push_back(*(data_ptr + d)) // value is copied into targetBytes
		: targetBytes->push_back(0); // otherwise zero padding is applied
}

void appendDataToBytes(const uint8_t* data_ptr, size_t dataSize, std::vector<uint8_t>* targetBytes){
    size_t paddingSize = PADDING_WIDTH - (dataSize % PADDING_WIDTH); // manually computed padding value
    alignDataToBytes(data_ptr, dataSize, paddingSize, targetBytes);
}

// Transformation Operations

static Mat4x4 genPerspectiveMatrix(SpatialBounds3D bounds){
	float r = bounds.right; float l = bounds.left;
	float t = bounds.top; float b = bounds.bottom;
	float n = bounds.nearPlane; float f = bounds.farPlane;

    /* Mat4x4 projMatrix = Mat4x4({ // From OpenGL SuperBible starting page 88
        (2.0f * n) / (r - l), 0.0f, (r + l) / (r - l), 0.0f,
        0.0f, (2.0f * n) / (t - b), (t + b) / (t - b), 0.0f,
        0.0f, 0.0f, (n + f) / (n - f), (2.0f * n * f) / (n - f),
        0.0f, 0.0f, -1.0f, 0.0f
    }); */
	Mat4x4 projMatrix = Mat4x4({ // From Game Engine Architecture page 437
		(2.0f * n) / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, (2.0f * n) / (t - b), 0.0f, 0.0f,
		(r + l) / (r - l), (t + b) / (t - b), -((f + n) / (f - n)), -1.0f,
		0.0f, 0.0f, -((2.0f * n * f) / (f - n)), 0.0f
	});
    return projMatrix;
}

static Mat4x4 genOrthoMatrix(SpatialBounds3D bounds){
	float r = bounds.right; float l = bounds.left;
	float t = bounds.top; float b = bounds.bottom;
	float n = bounds.nearPlane; float f = bounds.farPlane;

    Mat4x4 projMatrix = Mat4x4({ // From OpenGL SuperBible starting page 89
        2.0f / (r - l), 0.0f, 0.0f, (l + r) / (l - r),
        0.0f, 2.0f / (t - b), 0.0f, (b + t) / (b - t),
        0.0f, 0.0f, 2.0f / (n - f), (f + n) / (f - n),
        0.0f, 0.0f, 0.0f, 1.0f });
    return projMatrix;
}

static Mat4x4 genStereoMatrix(SpatialBounds3D bounds){
    return MAT_4x4_IDENTITY;
}

static Mat4x4 genGnomonicMatrix(SpatialBounds3D bounds){
    return MAT_4x4_IDENTITY;
}

Mat4x4 genProjMatrix(PROJECTION_Type type, const SpatialBounds3D& bounds){
    switch(type){
        case PROJECTION_Perspective: return genPerspectiveMatrix(bounds);
        case PROJECTION_Ortho: return genOrthoMatrix(bounds);
        case PROJECTION_Stereo: return genStereoMatrix(bounds);
        case PROJECTION_Gnomonic: return genGnomonicMatrix(bounds);
        // case PROJECTION_Test: return MAT_4x4_TEST;
    }
}