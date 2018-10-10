#include "real_time_renderer.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

#include <functional>

#include <glm/gtc/type_ptr.hpp>
#include "filesystem.hpp"
#include "shader.hpp"

namespace gl_code {
    int RealTimeRenderer::activeLights = 0;
    bool RealTimeRenderer::lightMoveMode = false;
    int RealTimeRenderer::lightMoveInd = 0;

    bool RealTimeRenderer::firstMouse = true;
    bool RealTimeRenderer::wPressed = false;
    bool RealTimeRenderer::sPressed = false;
    bool RealTimeRenderer::dPressed = false;
    bool RealTimeRenderer::aPressed = false;
    bool RealTimeRenderer::rPressed = false;
    bool RealTimeRenderer::fPressed = false;

    GLfloat RealTimeRenderer::lastX = 0;
    GLfloat RealTimeRenderer::lastY = 0;
    GLfloat RealTimeRenderer::xoffset = 0;
    GLfloat RealTimeRenderer::yoffset = 0;

    RealTimeRenderer::RealTimeRenderer() {
        mWidth = 800;
        mHeight = 800;
        mNear = 0.1f;
        mFar = 1000.f;

        NUM_LIGHTS = 3;
        NUM_AMB_PROBES = 30;

        testCoord = glm::vec3(-10, 12, -3);

        shadowFar = 100.0f;
        shadowNear = 1.0f;

        useTextures = true;

        camera = new Camera(glm::vec3(0.f, 0.f, 2.f));

        rands = std::uniform_real_distribution<GLfloat>(0, 1);
        eng = std::default_random_engine(time(NULL));

        renderMode = 0;

        shadowTexNames = {
                "shadowMap0",
                "shadowMap1",
                "shadowMap2"
        };

        cubeVAO = 0;
        cubeVBO = 0;
        cubeTex = 0;

        speed = 6;
        lastTime = -1.0;
        lastX = 400;
        lastY = 300;
        firstMouse = true;
    }

    int RealTimeRenderer::RenderLoop() {
        // Load GLFW and Create a Window
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

        // Check for Valid Context
        if (mWindow == nullptr) {
            fprintf(stderr, "Failed to Create OpenGL Context");
            return EXIT_FAILURE;
        }

        // Create Context and Load OpenGL Functions
        glfwMakeContextCurrent(mWindow);
        gladLoadGL();
        fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

        cout << "Mouse to rotate, WASD to move"
             << "\n\n\t- Press L to switch to light movement mode"
             << "\n\t\tWhile in this mode, press SPACE to switch lights"
             << "\n\t\tWASD will move the lights according to the player's orientation\n"
             << std::endl;

        using namespace std::placeholders;
        // Set callback functions
        glfwSetKeyCallback(mWindow, key_callback);
        glfwSetCursorPosCallback(mWindow, mouse_callback);
        glfwSetScrollCallback(mWindow, scroll_callback);
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        float am = .05f;

        /*
        lights.push_back(
                Light(glm::vec3(-30, 30, -3), glm::vec3(am, am, am), glm::vec3(1, .1, .1), glm::vec3(1, .1, .1), 0));
        lights.push_back(
                Light(glm::vec3(0, 10, 3), glm::vec3(am, am, am), glm::vec3(.1, .1, 1), glm::vec3(.1, .1, 1), 1));
        lights.push_back(
                Light(glm::vec3(30, 30, -3), glm::vec3(am, am, am), glm::vec3(.1, 1, .1), glm::vec3(.1, 1, .1), 2));
                */
        lights.push_back(
                Light(glm::vec3(-30, 30, -3), glm::vec3(am, am, am), glm::vec3(.5, .5, .5), glm::vec3(.5, .5, .5), 0));
        lights.push_back(
                Light(glm::vec3(-5, 15, 8), glm::vec3(am, am, am), glm::vec3(.5, .5, .5), glm::vec3(.5, .5, .5), 1));
        lights.push_back(
                Light(glm::vec3(0, 0, 0), glm::vec3(am, am, am), glm::vec3(.5, .5, .5), glm::vec3(.5, .5, .5), 2));
        activeLights = 3;

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_DEPTH_TEST);

        // Create a sample shader that displays normal
        fullShader = new Shader(FileSystem::getPath("Shaders/geometry.vert.glsl").c_str(),
                                FileSystem::getPath("Shaders/geometry.frag.glsl").c_str());
        shadowShader = new Shader(FileSystem::getPath("Shaders/shadow_geometry.vert.glsl").c_str(),
                                  FileSystem::getPath("Shaders/shadow_geometry.frag.glsl").c_str(),
                                  FileSystem::getPath("Shaders/shadow_geometry.geom.glsl").c_str());
        /*mapShader = Shader(FileSystem::getPath("Shaders/map_geometry.vert.glsl").c_str(),
            FileSystem::getPath("Shaders/map_geometry.frag.glsl").c_str(),
            FileSystem::getPath("Shaders/shadow_geometry.geom.glsl").c_str());*/

        cout << "About to load models" << endl;

        // Load a model from obj file
        models.push_back(Model(FileSystem::getPath("Resources/nano_suit/nanosuit.obj").c_str()));
        model_mats.push_back(glm::mat4(1));
        models.push_back(Model(FileSystem::getPath("Resources/crytek_sponza/sponza.obj").c_str()));
        model_mats.push_back(glm::scale(glm::mat4(1), glm::vec3(0.08f)));  // The sponza model is too big, scale it first

        cout << "Loaded models" << endl;

        fullShader->Use();
        for (int i = 0; i < lights.size(); i++) {
            lights[i].update(*fullShader);
        }

        cout << "Updated lights" << endl;

        for (int i = 0; i < NUM_LIGHTS; i++) {
            shadowMaps.push_back(0);
            shadowFrameBuffers.push_back(0);
            glGenTextures(1, &shadowMaps[i]);
            glGenFramebuffers(1, &shadowFrameBuffers[i]);
        }
        cout << "Set up shadow maps" << endl;

        cout << "Reached rendering loop" << endl;

        Shader *currentShader;
        // Rendering Loop
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();
            updatePosition();
            camera->ProcessMouseMovement(xoffset, yoffset);
            xoffset = 0;
            yoffset = 0;
            // Background Fill Color
            glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            currentShader = fullShader;
            fullShader->Use();

            glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom),
                                                    (GLfloat) mWidth / (GLfloat) mHeight, mNear,
                                                    mFar);
            glm::mat4 view = camera->GetViewMatrix();
            glUniformMatrix4fv(glGetUniformLocation(currentShader->Program, "projection"), 1, GL_FALSE,
                               glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(currentShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniform3fv(glGetUniformLocation(currentShader->Program, "pPos"), 1, glm::value_ptr(camera->Position));

            if (renderMode == 0) {
                for (int i = 0; i < NUM_LIGHTS; i++) {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glUniform1i(glGetUniformLocation(currentShader->Program, shadowTexNames[i].c_str()), i);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMaps[i]);
                }
            }

            useTextures = true;
            drawMainScene(currentShader);

            glm::mat4 model = glm::mat4(1);
            for (int i = 0; i < activeLights; i++) {
                glUniformMatrix4fv(glGetUniformLocation(currentShader->Program, "model"),
                                   1,
                                   GL_FALSE,
                                   glm::value_ptr(glm::translate(model, lights[i].getPos())));
                RenderCube(*currentShader);
            }

            glfwSwapBuffers(mWindow);
        }
        glfwTerminate();
        return EXIT_SUCCESS;
    }

    void RealTimeRenderer::setupShadowMatrices(int lightIndex) {
        glm::mat4 perspective = glm::perspective(glm::radians(90.0f), 1.0f, shadowNear, shadowFar);
        glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "shadowMat[0]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(lights[lightIndex].getPos(),
                                                             lights[lightIndex].getPos() + glm::vec3(1, 0, 0),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "shadowMat[1]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(lights[lightIndex].getPos(),
                                                             lights[lightIndex].getPos() + glm::vec3(-1, 0, 0),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "shadowMat[2]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(lights[lightIndex].getPos(),
                                                             lights[lightIndex].getPos() + glm::vec3(0, 1, 0),
                                                             glm::vec3(0, 0, 1))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "shadowMat[3]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(lights[lightIndex].getPos(),
                                                             lights[lightIndex].getPos() + glm::vec3(0, -1, 0),
                                                             glm::vec3(0, 0, -1))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "shadowMat[4]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(lights[lightIndex].getPos(),
                                                             lights[lightIndex].getPos() + glm::vec3(0, 0, 1),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(shadowShader->Program, "shadowMat[5]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(lights[lightIndex].getPos(),
                                                             lights[lightIndex].getPos() + glm::vec3(0, 0, -1),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniform3fv(glGetUniformLocation(shadowShader->Program, "lightPos"), 1,
                     glm::value_ptr(lights[lightIndex].getPos()));
        glUniform1f(glGetUniformLocation(shadowShader->Program, "farClip"), shadowFar);
    }

    void RealTimeRenderer::renderShadowMaps() {
        useTextures = false;
        shadowShader->Use();
        GLuint shadowRes = 1024;
        glViewport(0, 0, shadowRes, shadowRes);

        for (int lightNum = 0; lightNum < NUM_LIGHTS; lightNum++) {
            setupShadowMatrices(lightNum);
            glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMaps[lightNum]);
            for (GLuint i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                             shadowRes, shadowRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffers[lightNum]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMaps[lightNum], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glClear(GL_DEPTH_BUFFER_BIT);

            drawMainScene(shadowShader);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
        glViewport(0, 0, mWidth, mHeight);
        useTextures = true;
    }

    void RealTimeRenderer::drawMainScene(Shader *currentShader) {
        for(int i = 0; i < models.size(); i++){
            glUniformMatrix4fv(glGetUniformLocation(currentShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model_mats[i]));
            models[i].Draw(*currentShader, useTextures);
        }
    }

    // RenderCube() Renders a 1x1 3D cube in NDC.
    void RealTimeRenderer::RenderCube(const Shader &shader) {
        // Initialize (if necessary)
        if (cubeVAO == 0) {
            GLfloat vertices[] = {
                    // Back face
                    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
                    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
                    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
                    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
                    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
                    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,// top-left
                    // Front face
                    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
                    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
                    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
                    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
                    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
                    // Left face
                    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
                    -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
                    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
                    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
                    -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
                    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
                    // Right face
                    0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
                    0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
                    0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
                    0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
                    0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
                    0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
                    // Bottom face
                    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-right
                    0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-left
                    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
                    0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
                    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
                    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-right
                    // Top face
                    -0.5f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,// top-left
                    0.5f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
                    0.5f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-right
                    0.5f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
                    -0.5f, 0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,// top-left
                    -0.5f, 0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f // bottom-left
            };
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // Fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // Link vertex attributes
            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) (6 * sizeof(GLfloat)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            uint8_t texture[] = {
                    255, 255, 255};
            glGenTextures(1, &cubeTex);
            glBindTexture(GL_TEXTURE_2D, cubeTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &(texture[0]));
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // Render Cube

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader.Program, "texture_diffuse1"), 0);
        glBindTexture(GL_TEXTURE_2D, cubeTex);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RealTimeRenderer::updatePosition() {
        if (lastTime == -1.0) {
            lastTime = glfwGetTime();
            return;
        }

        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        if (lightMoveMode) {
            if (wPressed) {
                lights[lightMoveInd].ProcessKeyboard(FORWARD, speed * deltaTime);
            }
            if (sPressed) {
                lights[lightMoveInd].ProcessKeyboard(BACKWARD, speed * deltaTime);
            }
            if (aPressed) {
                lights[lightMoveInd].ProcessKeyboard(LEFT, speed * deltaTime);
            }
            if (dPressed) {
                lights[lightMoveInd].ProcessKeyboard(RIGHT, speed * deltaTime);
            }
            if (rPressed) {
                lights[lightMoveInd].ProcessKeyboard(UP, speed * deltaTime);
            }
            if (fPressed) {
                lights[lightMoveInd].ProcessKeyboard(DOWN, speed * deltaTime);
            }
            lights[lightMoveInd].update(*fullShader);
        } else {
            if (wPressed) {
                camera->ProcessKeyboard(FORWARD, speed * deltaTime);
            }
            if (sPressed) {
                camera->ProcessKeyboard(BACKWARD, speed * deltaTime);
            }
            if (aPressed) {
                camera->ProcessKeyboard(LEFT, speed * deltaTime);
            }
            if (dPressed) {
                camera->ProcessKeyboard(RIGHT, speed * deltaTime);
            }
            if (rPressed) {
                camera->ProcessKeyboard(UP, speed * deltaTime);
            }
            if (fPressed) {
                camera->ProcessKeyboard(DOWN, speed * deltaTime);
            }
        }
    }

// Is called whenever a key is pressed/released via GLFW
    void RealTimeRenderer::key_callback(
            GLFWwindow *window,
            int key,
            int scancode,
            int action,
            int mode) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        // Camera movements
        switch (key) {
            case GLFW_KEY_W:
                wPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_S:
                sPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_A:
                aPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_D:
                dPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_R:
                rPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_F:
                fPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_L:
                if (action == GLFW_PRESS) {
                    lightMoveMode = !lightMoveMode;
                }
                break;
            case GLFW_KEY_SPACE:
                if (lightMoveMode && action == GLFW_PRESS) {
                    lightMoveInd = (lightMoveInd + 1) % activeLights;
                }
                break;
        }
    }

    void RealTimeRenderer::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        xoffset += xpos - lastX;
        yoffset += lastY - ypos;

        lastX = xpos;
        lastY = ypos;
    }

    void RealTimeRenderer::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        //camera->ProcessMouseScroll(yoffset);
    }

}  // namespace gl_code