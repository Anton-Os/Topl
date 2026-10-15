// Audio spectrum access for the spectrum pipeline
// params[0..3]: 16 band levels in range 0 to 1
// params[4]: x bass, y mids, z treble, w overall level

#define SPECTRUM_BANDS 16

float getBand(int index){
	int b = clamp(index, 0, SPECTRUM_BANDS - 1);
	return params[b / 4][b % 4];
}

float getBandSmooth(float position){ // continuous lookup between neighboring bands
	float p = clamp(position, 0.0, float(SPECTRUM_BANDS - 1));
	int i = int(floor(p));
	return lerp(getBand(i), getBand(i + 1), frac(p));
}

float getBass(){ return params[4].x; }
float getMids(){ return params[4].y; }
float getTreble(){ return params[4].z; }
float getLevel(){ return params[4].w; }

float3 getBandColor(float position){ return hsv2rgb(float3(0.62 - (position / float(SPECTRUM_BANDS)) * 0.62 + getTime() * 0.01, 0.85, 1.0)); }
