#ifndef SCENE_STRUCTURE_H
#define SCENE_STRUCTURE_H

#include <glm/glm.hpp>
#include "tracer_structs.h"
#include "util.h"
#include "scene_io.h"
#include "BoundingBox.h"
#include <iostream>
#include "PolyBound.h"
#include "ObjBound.h"

void BuildAccelerators(SceneData& scene_data, RayTracerParams& params);

void jacksBuildBounds(SceneData& scene_data);

void jacksCleanupBounds(SceneData& scene_data);

#endif // !SCENE_STRUCTURE_H
