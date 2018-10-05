//
// Created by jalexander on 10/4/18.
//

#ifndef GLITTER_OPENGL_STRUCTS_H
#define GLITTER_OPENGL_STRUCTS_H

#include <glm/glm.hpp>
#include "light.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "camera.hpp"
#include <memory>
#include <vector>
#include <random>

namespace gl_code {

    struct OpenglVars {
        int mWidth;
        int mHeight;
        float mNear;
        float mFar;

        int NUM_LIGHTS;
        int NUM_AMB_PROBES;

        std::vector<glm::vec3> ambProbes;

        std::vector<Light> lights;
        int activeLights;
        bool lightMoveMode;
        int lightMoveInd;

        glm::vec3 testCoord;

        bool wPressed;
        bool sPressed;
        bool dPressed;
        bool aPressed;

        Shader fullShader;
        Model sampleModel;
        std::vector<GLuint> shadowMaps;
        std::vector<GLuint> shadowFrameBuffers;
        Shader shadowShader;
        Shader ssaoShader;
        Shader ssdoShader;
        Shader depthShader;
        Shader mapShader;
        GLuint ambPositionTex;
        GLuint ambPositionBuffer;
        GLuint ambPositionRenderBuffer;

        GLuint mainMapTex;

        float shadowFar;
        float shadowNear;

        bool useTextures;

        Camera camera;

        std::uniform_real_distribution<GLfloat> rands;

        std::default_random_engine eng;

        int renderMode;

        std::vector<char *>shadowTexNames;

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
