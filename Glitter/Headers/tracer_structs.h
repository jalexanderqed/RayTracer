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
    int IMAGE_WIDTH;
    int IMAGE_HEIGHT;
    float EPSILON;
    float EPS_FACTOR;

    int numThreads;
    bool useAcceleration;
    bool complexColorShaders;
    bool complexIntersectShaders;

    int SAMPLES_PER_PIXEL;
    float lensSide;
    float focalLength;
    float globalFocalDistance;
};

struct SceneData {
    char* sceneName;
    std::unordered_map<const ObjIO *, int> sphereMap;

    float *image;

    SceneIO *sceneGeometry;
    std::list<ObjBound *> boundBoxes;
};

#endif //GLITTER_TRACER_STRUCTS_H
