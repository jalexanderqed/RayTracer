//
// Created by jalex on 10/6/2018.
//

#include "light.hpp"

#include "shader.hpp"

using gl_code::Shader;

namespace shared_obj {

    Light::Light(const glm::vec3& pos, const glm::vec3& color, int ind) {
            position_ = pos;
            color_ = color;
            index_ = ind;
        type_ = POINT_LIGHT;
        myPre_ = "pointLights[" + std::to_string(index_) + "]";
    }

    Light::Light(LightIO *l, int ind) {
        position_ = glm::vec3(l->position[0], l->position[1], l->position[2]);
        direction_ = glm::vec3(l->direction[0], l->direction[1], l->direction[2]);
        color_ = glm::vec3(l->color[0], l->color[1], l->color[2]);
        type_ = l->type;
        index_ = ind;
        myPre_ = "pointLights[" + std::to_string(index_) + "]";
    }

    void Light::update(const Shader &shader) {
        glUniform3fv(glGetUniformLocation(shader.Program, (myPre_ + ".position").c_str()), 1,
                     glm::value_ptr(position_));
        glUniform3fv(glGetUniformLocation(shader.Program, (myPre_ + ".color").c_str()), 1,
                     glm::value_ptr(color_));
    }

    void Light::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
        glm::vec3 front(1, 0, 0);
        glm::vec3 right(0, 0, 1);
        glm::vec3 up(0, 1, 0);
        GLfloat velocity = 3 * deltaTime;
        switch (direction) {
            case FORWARD:
                this->position_ += front * velocity;
                break;
            case BACKWARD:
                this->position_ -= front * velocity;
                break;
            case LEFT:
                this->position_ -= right * velocity;
                break;
            case RIGHT:
                this->position_ += right * velocity;
                break;
            case UP:
                this->position_ += up * velocity;
                break;
            case DOWN:
                this->position_ -= up * velocity;
                break;
        }
    }

}  // namspace shared_obj
