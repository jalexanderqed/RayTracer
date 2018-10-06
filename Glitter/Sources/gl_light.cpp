//
// Created by jalex on 10/6/2018.
//

#include "light.hpp"

namespace gl_code {

    Light::Light() :
            position(glm::vec3()),
            ambient(glm::vec3()),
            diffuse(glm::vec3()),
            specular(glm::vec3()),
            index(0) {}

    Light::Light(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, int ind) :
            position(pos),
            ambient(amb),
            diffuse(diff),
            specular(spec),
            index(ind) {
        std::ostringstream pred;
        pred << "pointLights[" << index << "]";
        myPre = pred.str();
    }

    void Light::update(const Shader &shader) {
        glUniform3fv(glGetUniformLocation(shader.Program, (myPre + ".position").c_str()), 1,
                     glm::value_ptr(position));
        glUniform3fv(glGetUniformLocation(shader.Program, (myPre + ".ambient").c_str()), 1,
                     glm::value_ptr(ambient));
        glUniform3fv(glGetUniformLocation(shader.Program, (myPre + ".diffuse").c_str()), 1,
                     glm::value_ptr(diffuse));
        glUniform3fv(glGetUniformLocation(shader.Program, (myPre + ".specular").c_str()), 1,
                     glm::value_ptr(specular));
    }

    glm::vec3 Light::getPos() {
        return position;
    }

    void Light::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime, glm::vec3 front, glm::vec3 right) {
        GLfloat velocity = 3 * deltaTime;
        if (direction == FORWARD)
            this->position += front * velocity;
        if (direction == BACKWARD)
            this->position -= front * velocity;
        if (direction == LEFT)
            this->position -= right * velocity;
        if (direction == RIGHT)
            this->position += right * velocity;
    }

}  // namspace gl_code
