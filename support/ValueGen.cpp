#include "ValueGen.hpp"

// Data Operations


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

Projection::Projection(PROJECTION_Type p, float s) {
	type = p;
	left *= s; right *= s;
	bottom *= s; top *= s;
	nearPlane *= s; farPlane *= s;
}

Projection::Projection(PROJECTION_Type p, float l, float r, float b, float t, float n, float f) {
	type = p;
	left *= l; right *= r;
	bottom *= b; top *= t;
	nearPlane *= n; farPlane *= f;
}

Mat4x4 Projection::genProjMatrix(){
	Mat4x4 projMatrix = MAT_4x4_IDENTITY;

	if(type == PROJECTION_Ortho){
		nearPlane *= PPROJ_Z; farPlane *= PPROJ_Z;
		
		projMatrix = Mat4x4({ // From OpenGL SuperBible starting page 89
			2.0f / (right - left), 0.0f, 0.0f, (left + right) / (left - right),
			0.0f, 2.0f / (top - bottom), 0.0f, (bottom + top) / (bottom - top),
			0.0f, 0.0f, 2.0f / (nearPlane - farPlane), (farPlane + nearPlane) / (farPlane - nearPlane),
			0.0f, 0.0f, 0.0f, 1.0f 
		});
	} else if(type == PROJECTION_Perspective){
		left /= PPROJ_A * -1.0; right /= PPROJ_A * -1.0;
		bottom /= PPROJ_A * -1.0; top /= PPROJ_A * -1.0;
		nearPlane /= PPROJ_A / (PPROJ_Z * 0.5F); farPlane /= PPROJ_A / (PPROJ_Z * 0.5F); 

		projMatrix = Mat4x4({ // From OpenGL SuperBible starting page 88
			(2.0f * nearPlane) / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
			0.0f, (2.0f * nearPlane) / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
			0.0f, 0.0f, -((farPlane + nearPlane) / (farPlane - nearPlane)), -((2.0f * nearPlane * farPlane) / (farPlane - nearPlane)),
			0.0f, 0.0f, -1.0f, 0.0f
		});
	} else {
		projMatrix = Mat4x4({
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		});
	}

	return projMatrix;
}