#include "ray_tracer.h"

void loadScene(SceneData &scene_data) {
    /* load the scene into the SceneIO data structure using given parsing code */
    scene_data.sceneGeometry = readScene(scene_data.sceneName);
    if (scene_data.sceneGeometry == NULL) exit(1);
}

void updateRayTracerParams(SceneData &scene_data, RayTracerParams &scene_params) {
    // Calculates a custom epsilon value based on size of the scene
    BoundingBox sceneBox = boundScene(scene_data.sceneGeometry);
    float diffBound = max(sceneBox.vMax.x - sceneBox.vMin.x,
                          max(sceneBox.vMax.y - sceneBox.vMin.y, sceneBox.vMax.z - sceneBox.vMin.z)) / 2.0f;
    float posBound = max(sceneBox.vMax.x,
                         max(sceneBox.vMax.y, sceneBox.vMax.z));
    float negBound = min(sceneBox.vMin.x,
                         min(sceneBox.vMin.y, sceneBox.vMin.z));
    float largeBound = max(abs(posBound), max(abs(negBound), diffBound));

    // EPS_FACTOR was determined with testing of different epsilon values on scenes
    scene_params.EPSILON = min(largeBound / scene_params.EPS_FACTOR, 0.0001f);

    if (scene_params.SAMPLES_PER_PIXEL != 1) {
        int len = strlen(scene_data.sceneName);
        scene_params.globalFocalDistance = scene_data.sceneGeometry->camera->focalDistance;
        switch (scene_data.sceneName[len - 7]) {
            case '3':
                scene_params.lensSide = 50;
                break;
            case '5':
                scene_params.lensSide = 100;
                break;
        }
    }
}

void render(SceneData& scene_data, RayTracerParams& scene_params) {
    scene_data.image = new float[sizeof(float) * scene_params.IMAGE_HEIGHT * scene_params.IMAGE_WIDTH * 3];

    jacksRenderScene(scene_data, scene_params);
    return;
}

inline void cross(const glm::vec3 &v1, float *v2, float *res) {
    res[0] = v1[1] * v2[2] - v1[2] * v2[1];
    res[1] = v1[2] * v2[0] - v1[0] * v2[2];
    res[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
