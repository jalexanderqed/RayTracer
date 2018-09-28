//
// Created by jalexander on 9/26/18.
//

#ifndef GLITTER_RAY_TRACER_H
#define GLITTER_RAY_TRACER_H

#include <glm/glm.hpp>
#include <list>
#include <unordered_map>

#include "ObjBound.h"
#include "scene_io.h"

const int IMAGE_WIDTH = 1500;
const int IMAGE_HEIGHT = 1500;
float EPSILON = 0.00005f;
const float EPS_FACTOR = 67000;

int numThreads = 8;
bool useAcceleration = true;
bool complexColorShaders = false;
bool complexIntersectShaders = false;

const int SAMPLES_PER_PIXEL = 1;
float lensSide = 5.0f;
float focalLength = 0.01f;
float globalFocalDistance = 2;

unordered_map<const ObjIO*, int> sphereMap;

using namespace std;

float *image;

SceneIO *scene = NULL;
list<ObjBound*> boundBoxes;

static void loadScene(char *name);

void render(void);

inline void cross(const glm::vec3& v1, float* v2, float* res);

void jacksRenderScene(SceneIO* scene);

void jacksBuildBounds(SceneIO* scene);

#endif //GLITTER_RAY_TRACER_H
