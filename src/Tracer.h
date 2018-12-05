#ifndef TRACER_H
#define TRACER_H

#define _USE_MATH_DEFINES

#include <glm/glm.hpp>
#include <tracer_structs.h>
#include "util.h"
#include "Intersections.h"
#include "SceneCamera.h"
#include "light.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include "SceneStructure.h"
#include <unordered_map>
#include <thread>

void jacksRenderScene(SceneData& scene_data, RayTracerParams& scene_params);

#endif // !TRACER_H
