#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_const.h"
#include "scene_io.h"

namespace shared_obj{
    class Shader;
}

namespace shared_obj {

    enum LightType {POINT_LIGHT, DIRECTIONAL_LIGHT, SPOT_LIGHT};

    class Light {
    private:
        glm::vec3 position_;
        glm::vec3 color_;
        LightType type_;
        glm::vec3 direction_;
        size_t index_;
        std::string myPre_;

    public:
        Light(const glm::vec3& pos, const glm::vec3& color, size_t ind, LightType type);

        Light(const glm::vec3& pos, glm::vec3& direction, const glm::vec3& color, size_t ind, LightType type);

        void update(const shared_obj::Shader &shader);

        const glm::vec3 &position() { return position_; }

        const glm::vec3 &color() { return color_; }

        const glm::vec3 &direction() {return direction_;}

        LightType light_type() { return type_; }

        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
    };

}  // namspace shared_obj

#endif
