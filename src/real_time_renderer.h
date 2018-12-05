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
#include <time.h>

#include "light.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "model.hpp"

namespace shared_obj {

    class RealTimeRenderer {
    public:

        RealTimeRenderer();

        int RenderLoop();
    private:
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

        static void mouse_callback(GLFWwindow *window, double xpos, double ypos);

        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

        void updatePosition();

        void drawMainScene(Shader *shader);

        void setupShadowMatrices(int lightIndex);

        void renderShadowMaps();

        void RenderCube(const Shader &shader);

        int mWidth;
        int mHeight;
        float mNear;
        float mFar;

        int NUM_LIGHTS;
        int NUM_AMB_PROBES;

        std::vector<shared_obj::Light> lights;

        glm::vec3 testCoord;

        Shader *fullShader;

        vector<Model> models;
        vector<glm::mat4> model_mats;

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

        std::vector<string> shadowTexNames;

        GLuint cubeVAO;
        GLuint cubeVBO;
        GLuint cubeTex;

        double speed;
        double lastTime;

        static int activeLights;
        static bool lightMoveMode;
        static int lightMoveInd;

        static bool firstMouse;
        static bool wPressed;
        static bool sPressed;
        static bool dPressed;
        static bool aPressed;
        static bool rPressed;
        static bool fPressed;

        static GLfloat lastX;
        static GLfloat lastY;
        static GLfloat xoffset;
        static GLfloat yoffset;
    };

}  // namespace shared_obj

#endif //GLITTER_GL_RENDER_FUNCS_H
