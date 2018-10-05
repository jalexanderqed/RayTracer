//
// Created by jalexander on 10/4/18.
//

#ifndef GLITTER_GL_RENDER_FUNCS_H
#define GLITTER_GL_RENDER_FUNCS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void updatePosition();
void drawMainScene(Shader* shader);
void renderShadowMaps();
void RenderCube(const Shader& shader);
void renderDepthTex();
void setupAmbProbes();
void renderMainMap();

#endif //GLITTER_GL_RENDER_FUNCS_H
