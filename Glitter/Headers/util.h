#ifndef UTIL_H
#define UTIL_H

#include "glitter.h"
#include <algorithm>
#include "scene_io.h"

extern float EPSILON;

using namespace std;

extern const int IMAGE_WIDTH;
extern const int IMAGE_HEIGHT;

extern float* image;

void setPixel(int x, int y, const glm::vec3& color);

glm::vec3 getPixel(int x, int y);

glm::vec3 reflect(const glm::vec3& out, const glm::vec3& normal);

glm::vec3 refract(const glm::vec3& out, const glm::vec3& normal, float oldIR, float newIR);

#endif