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

namespace gl_code{
    class Shader;
}

namespace shared_obj {

    class Light {
    private:
        glm::vec3 position_;
        glm::vec3 color_;
        LightType type_;
        glm::vec3 direction_;
        int index_;
        std::string myPre_;
        float drop_off_rate_;
        float cut_off_angle_;

    public:
        Light();

        Light(glm::vec3 pos, glm::vec3 color, int ind);

        Light(LightIO *l);

        void update(const gl_code::Shader &shader);

        const glm::vec3 &position() { return position_; }

        const glm::vec3 &color() { return color_; }

        const glm::vec3 &direction() {return direction_;}

        LightType light_type() { return type_; }

        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
    };

}  // namspace shared_obj

#endif
