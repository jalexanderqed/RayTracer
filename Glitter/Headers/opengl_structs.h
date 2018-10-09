//
// Created by jalexander on 10/4/18.
//

#ifndef GLITTER_OPENGL_STRUCTS_H
#define GLITTER_OPENGL_STRUCTS_H

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <random>

#include "light.hpp"
#include "gl_const.h"

namespace gl_code {

    class Shader;

    class Camera;

    class Model;

    struct OpenglVars {
        int mWidth;
        int mHeight;
        float mNear;
        float mFar;

        int NUM_LIGHTS;
        int NUM_AMB_PROBES;

        std::vector<Light> lights;
        int activeLights;
        bool lightMoveMode;
        int lightMoveInd;

        glm::vec3 testCoord;

        bool wPressed;
        bool sPressed;
        bool dPressed;
        bool aPressed;
        bool rPressed;
        bool fPressed;

        Shader *fullShader;
        Model *sampleModel;
        std::vector<GLuint> shadowMaps;
        std::vector<GLuint> shadowFrameBuffers;
        Shader *shadowShader;
        Shader *mapShader;

        GLuint mainMapTex;

        float shadowFar;
        float shadowNear;

        bool useTextures;

        Camera *camera;

        std::uniform_real_distribution<GLfloat> rands;

        std::default_random_engine eng;

        int renderMode;

        std::vector<char *> shadowTexNames;

        GLuint cubeVAO;
        GLuint cubeVBO;
        GLuint cubeTex;

        double speed;
        double lastTime;

        GLfloat lastX;
        GLfloat lastY;
        bool firstMouse;
    };

}  // namespace gl_code

#endif //GLITTER_OPENGL_STRUCTS_H
