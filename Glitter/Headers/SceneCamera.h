#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include <glm/glm.hpp>
#include <scene_io.h>
#include "tracer_structs.h"

class SceneCamera {
public:
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 forward;
	glm::vec3 pos;
	glm::vec3 lens;
	glm::vec3 screenPos;
	glm::vec3 screenVert;
	glm::vec3 screenHoriz;
	float focalDistance;
	float vertFov;
	float horizFov;

	SceneCamera(CameraIO* camera, RayTracerParams scene_params) {
		up = glm::normalize(glm::vec3(camera->orthoUp[0], camera->orthoUp[1], camera->orthoUp[2]));
		forward = glm::normalize(glm::vec3(camera->viewDirection[0], camera->viewDirection[1], camera->viewDirection[2]));
		right = glm::normalize(glm::cross(forward, up));
		up = glm::normalize(glm::cross(right, forward));
		pos = glm::vec3(camera->position[0], camera->position[1], camera->position[2]);
		screenPos = pos + scene_params.focalLength * forward;
		lens = screenPos + scene_params.focalLength * forward;
		focalDistance = scene_params.globalFocalDistance;
		vertFov = camera->verticalFOV;
		horizFov = (((float)scene_params.IMAGE_WIDTH) / scene_params.IMAGE_HEIGHT) * vertFov;
		screenVert = scene_params.focalLength * (float)tan(vertFov / 2.0f) * up;
		screenHoriz = scene_params.focalLength * (float)tan(horizFov / 2.0f) * right;
	}
};

#endif // !SCENE_CAMERA_H
