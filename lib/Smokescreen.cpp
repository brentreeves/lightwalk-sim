#include "standardEffect.h"
#include "application.h"
#include <random>

//#include <math.h>

//#include <cstdlib>
//#include <ctime>

//PerlinNoiseCpp by Chris Litle from:
//https://github.com/captainhead/PerlinNoiseCpp
class Perlin {
public:
  Perlin(){
    //srand(time(NULL));

    p = new int[256];
    Gx = new float[256];
    Gy = new float[256];
    Gz = new float[256];

    for (int i=0; i<256; ++i) {
      p[i] = i;

      Gx[i] = (float(perlinRandom(0,RAND_MAX)) / (RAND_MAX/2)) - 1.0f;
      Gy[i] = (float(perlinRandom(0,RAND_MAX)) / (RAND_MAX/2)) - 1.0f;
      Gz[i] = (float(perlinRandom(0,RAND_MAX)) / (RAND_MAX/2)) - 1.0f;
    }

    int j=0;
    int swp=0;
    for (int i=0; i<256; i++) {
      j = perlinRandom(0,RAND_MAX) & 255;

      swp = p[i];
      p[i] = p[j];
      p[j] = swp;
    }
  }

  ~Perlin(){
    delete p;
    delete Gx;
    delete Gy;
    delete Gz;
  }

  // Generates a Perlin (smoothed) noise value between -1 and 1, at the given 3D position.
  float noise(float sample_x, float sample_y, float sample_z){

    // Unit cube vertex coordinates surrounding the sample point
    int x0 = int(floor(sample_x));
    int x1 = x0 + 1;
    int y0 = int(floor(sample_y));
    int y1 = y0 + 1;
    int z0 = int(floor(sample_z));
    int z1 = z0 + 1;

    // Determine sample point position within unit cube
    float px0 = sample_x - float(x0);
    float px1 = px0 - 1.0f;
    float py0 = sample_y - float(y0);
    float py1 = py0 - 1.0f;
    float pz0 = sample_z - float(z0);
    float pz1 = pz0 - 1.0f;

    // Compute dot product between gradient and sample position vector
    int gIndex = p[(x0 + p[(y0 + p[z0 & 255]) & 255]) & 255];
    float d000 = Gx[gIndex]*px0 + Gy[gIndex]*py0 + Gz[gIndex]*pz0;
    gIndex = p[(x1 + p[(y0 + p[z0 & 255]) & 255]) & 255];
    float d001 = Gx[gIndex]*px1 + Gy[gIndex]*py0 + Gz[gIndex]*pz0;
    
    gIndex = p[(x0 + p[(y1 + p[z0 & 255]) & 255]) & 255];
    float d010 = Gx[gIndex]*px0 + Gy[gIndex]*py1 + Gz[gIndex]*pz0;
    gIndex = p[(x1 + p[(y1 + p[z0 & 255]) & 255]) & 255];
    float d011 = Gx[gIndex]*px1 + Gy[gIndex]*py1 + Gz[gIndex]*pz0;
    
    gIndex = p[(x0 + p[(y0 + p[z1 & 255]) & 255]) & 255];
    float d100 = Gx[gIndex]*px0 + Gy[gIndex]*py0 + Gz[gIndex]*pz1;
    gIndex = p[(x1 + p[(y0 + p[z1 & 255]) & 255]) & 255];
    float d101 = Gx[gIndex]*px1 + Gy[gIndex]*py0 + Gz[gIndex]*pz1;

    gIndex = p[(x0 + p[(y1 + p[z1 & 255]) & 255]) & 255];
    float d110 = Gx[gIndex]*px0 + Gy[gIndex]*py1 + Gz[gIndex]*pz1;
    gIndex = p[(x1 + p[(y1 + p[z1 & 255]) & 255]) & 255];
    float d111 = Gx[gIndex]*px1 + Gy[gIndex]*py1 + Gz[gIndex]*pz1;

    // Interpolate dot product values at sample point using polynomial interpolation 6x^5 - 15x^4 + 10x^3
    float wx = ((6*px0 - 15)*px0 + 10)*px0*px0*px0;
    float wy = ((6*py0 - 15)*py0 + 10)*py0*py0*py0;
    float wz = ((6*pz0 - 15)*pz0 + 10)*pz0*pz0*pz0;

    float xa = d000 + wx*(d001 - d000);
    float xb = d010 + wx*(d011 - d010);
    float xc = d100 + wx*(d101 - d100);
    float xd = d110 + wx*(d111 - d110);
    float ya = xa + wy*(xb - xa);
    float yb = xc + wy*(xd - xc);
    float value = ya + wz*(yb - ya);

    return value;
  }

  //exact same from application.h, but in case things act funky, going to include it here. Calling it "perlinRandom()"
  int perlinRandom (int from, int daEnd) { 
  //  std::cout << "random " << from << ", " << daEnd << std::endl; 
  std::random_device rd;  //Will be used to obtain a seed for the random number engine 
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd() 
  std::uniform_int_distribution<> dis(from, daEnd); 
  //  std::cout << "random returning." << std::endl; 
  return dis(gen); 
  }

private:
  int *p; // Permutation table
  // Gradient vectors
  float *Gx;
  float *Gy;
  float *Gz;
};

class Smokescreen : public StandardEffect {
public:
  Smokescreen(int id, long startTime, int r, int g, int b, int sp) : StandardEffect(id, startTime) {
    _r = r;
    _g = g;
    _b = b;

    int speedDiff = _mapFloat(sp, 1, 10, 0, 10000);
    _duration = 20000 - speedDiff;

	//fadein at beginning?
  }

  bool cares(int x, int y) {
    return true;
  }

  uint32_t colorFor(int x, int y) {
  	float tempX = _mapFloat(x, 1, 10, 0, 1.5);
  	float tempY = _mapFloat(y, 1, PIXEL_COUNT, 1, 7);

  	//Can I get it to wobble back and forth as well?
  	//float wobbleX = _mapFloat(p.noise(x, y + .5, 1), -1, 1, -100, 100);
  	//Doesn't matter too much if rods are not all straight up

  	float _intensity = _mapFloat(p.noise(tempX + _xOffset, tempY + count, 1), -1, 1, 0, 1);

	 // tweek contrast
   	if (_intensity <= .5)
	   _intensity = _mapFloat(_intensity, 0, .5, 0, .1);
	 else
	   _intensity = _mapFloat(_intensity, .5, 1, .3, 1);

	 if (_intensity >= .45)
	   _intensity = _mapFloat(_intensity, .45, 1, .6, 1);

    int relativeR = _r * _intensity;
    int relativeG = _g * _intensity;
    int relativeB = _b * _intensity;

    return _rgbToColor(relativeR, relativeG, relativeB);
  }

private:
  void _update() {

  	int timePassed = (_currentTime - _startTime) % _duration;
  	count = _mapFloat(timePassed, 0, _duration, PIXEL_COUNT, 0);
  	//When gets to 0, jumps back to PIXEL_COUNT, and therefore the image "jitters"
  }

  Perlin p;
  int _r;
  int _g;
  int _b;
  float count = PIXEL_COUNT;
  int _duration;
};