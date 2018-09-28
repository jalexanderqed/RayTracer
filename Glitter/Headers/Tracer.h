#ifndef TRACER_H
#define TRACER_H

#define _USE_MATH_DEFINES

#include <glm/glm.hpp>
#include "ray_tracer.h"
#include "util.h"
#include "Intersections.h"
#include "SceneCamera.h"
#include "Light.h"
#include "scene_io.h"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include "SceneStructure.h"
#include <unordered_map>

using namespace std;

extern const int IMAGE_WIDTH;
extern const int IMAGE_HEIGHT;
extern const int SAMPLES_PER_PIXEL;
extern float EPSILON;
extern float lensSide;
extern int numThreads;
extern bool complexColorShaders;
extern unordered_map<const ObjIO*, int> sphereMap;

extern list<ObjBound*> boundBoxes;

void jacksRenderScene(SceneIO* scene);

struct ThreadData {
	int threadNum;
	SceneIO* scene;
};

#endif // !TRACER_H
