#include "SceneStructure.h"

void BuildAccelerators(SceneData& scene_data, RayTracerParams& params){
    if (params.complexIntersectShaders && params.complexColorShaders) {
        int count = 0;
        for (ObjIO *object = scene_data.sceneGeometry->objects;
             object != NULL;
             object = object->next) {
            if (object->type == SPHERE_OBJ) {
                scene_data.sphereMap.insert({object, count});
                count++;
            }
        }
    }

    // Builds accelleration structures
    jacksBuildBounds(scene_data);
}

void jacksBuildBounds(SceneData &scene_data) {
    for (ObjIO *object = scene_data.sceneGeometry->objects;
         object != NULL;
         object = object->next) {
        ObjBound *bound = new ObjBound(object);
        bound->split();
        scene_data.boundBoxes.push_back(bound);
    }
}

void jacksCleanupBounds(SceneData& scene_data) {
    for (ObjBound *o : scene_data.boundBoxes) {
        delete o;
    }
}