//
// Created by jalexander on 10/11/18.
//

#ifndef GLITTER_SCENE_H
#define GLITTER_SCENE_H

#include <vector>

#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "sphere.h"

namespace shared_obj {

    struct Scene {
        Camera camera;
        std::vector<Light> lights;
        std::vector<Model> models;
        std::vector<Sphere> spheres;
    };

}  // namespace shared_obj

#endif //GLITTER_SCENE_H
