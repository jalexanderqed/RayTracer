//
// Created by jalexander on 10/1/18.
//

#ifndef GLITTER_TRACER_STRUCTS_H
#define GLITTER_TRACER_STRUCTS_H

#include <unordered_map>
#include <list>

class SceneIO;
class ObjIO;
class ObjBound;

struct RayTracerParams {
    int IMAGE_WIDTH = 0;
    int IMAGE_HEIGHT = 0;
    float EPSILON = 0;
    float EPS_FACTOR = 0;

    int numThreads = 0;
    bool useAcceleration = false;
    bool complexColorShaders = false;
    bool complexIntersectShaders = false;

    int SAMPLES_PER_PIXEL = 0;
    float lensSide = 0;
    float focalLength = 0;
    float globalFocalDistance = 0;
};

struct SceneData {
    char* sceneName = nullptr;
    std::unordered_map<const ObjIO *, int> sphereMap;

    float *image = nullptr;

    SceneIO *sceneGeometry = nullptr;
    std::list<ObjBound *> boundBoxes;

    // Testing only
    int green_counted = 0;
    int red_counted = 0;
    int blue_counted = 0;
    int transparent = 0;
};

#endif //GLITTER_TRACER_STRUCTS_H
