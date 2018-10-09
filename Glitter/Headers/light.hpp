#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <iostream>

#include "gl_const.h"

namespace gl_code {

    class Shader;

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

        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
    };

}  // namspace gl_code

#endif
