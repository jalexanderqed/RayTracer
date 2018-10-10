#ifndef UTIL_H
#define UTIL_H

#include <glm/glm.hpp>
#include <algorithm>
#include "tracer_structs.h"
#include "scene_io.h"
#include "ray_tracer.h"

void setPixel(int x, int y, const glm::vec3& color, SceneData scene_data, RayTracerParams scene_params);

glm::vec3 getPixel(int x, int y, SceneData scene_data, RayTracerParams scene_params);

glm::vec3 reflect(const glm::vec3& out, const glm::vec3& normal);

glm::vec3 refract(const glm::vec3& out, const glm::vec3& normal, float oldIR, float newIR);

int write_img(const string& file_name, SceneData scene_data, RayTracerParams scene_params);

int write_img(const string& file_name, unsigned char* image, int width, int height);

#endif