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
#include "SceneStructure.h"
#include "tracer_structs.h"
#include "Tracer.h"

void loadScene(SceneData& scene_data);

void updateRayTracerParams(SceneData& scene_data, RayTracerParams& scene_params);

void render(SceneData& scene_data, RayTracerParams& scene_params);

inline void cross(const glm::vec3 &v1, float *v2, float *res);

#endif //GLITTER_RAY_TRACER_H
