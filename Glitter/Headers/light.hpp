#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"
#include "camera.hpp"

#include <string>
#include <sstream>
#include <iostream>

namespace gl_code {

    class Light {
    private:
        glm::vec3 position;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        int index;
        std::string myPre;

    public:
        Light();

        Light(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, int ind);

        void update(const Shader &shader);

        glm::vec3 getPos();

        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime, glm::vec3 front, glm::vec3 right);
    };

}  // namspace gl_code

#endif
