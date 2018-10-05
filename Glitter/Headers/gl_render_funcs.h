//
// Created by jalexander on 10/4/18.
//

#ifndef GLITTER_GL_RENDER_FUNCS_H
#define GLITTER_GL_RENDER_FUNCS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>
#include <random>

#include "opengl_structs.h"
#include "opengl_const.h"
#include "light.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "camera.hpp"

namespace gl_code {

    void init_vars(OpenglVars &vars);

    int gl_main_func(OpenglVars &vars);

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    void mouse_callback(GLFWwindow *window, double xpos, double ypos);

    void updatePosition(OpenglVars& vars);

    void drawMainScene(Shader *shader);

    void renderShadowMaps(OpenglVars& vars);

    void RenderCube(const Shader &shader, OpenglVars& vars);

    void renderDepthTex(OpenglVars& vars);

    void setupAmbProbes(OpenglVars& vars);

    void renderMainMap(OpenglVars& vars);

}  // namespace gl_code

#endif //GLITTER_GL_RENDER_FUNCS_H
