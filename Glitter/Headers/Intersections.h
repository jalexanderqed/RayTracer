#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include <glm/glm.hpp>
#include <ray_tracer.h>
#include "util.h"
#include "scene_io.h"
#include <iostream>
#include <cmath>
#include "IntersectionPrimitives.h"
#include "ObjBound.h"
#include <list>

using namespace std;

MaterialIO dupMaterial(const MaterialIO *material);

MaterialIO interpolateMaterials(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3,
                                const MaterialIO *material1, const MaterialIO *material2, const MaterialIO *material3,
                                const glm::vec3 &point);

IntersectionPoint intersectScene(
        const glm::vec3 &vec,
        const glm::vec3 &origin,
        SceneData &scene_data,
        RayTracerParams &scene_params
);

glm::vec3 getNormal(IntersectionPoint iPoint);

#endif