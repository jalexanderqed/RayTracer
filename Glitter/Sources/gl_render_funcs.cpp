#include "gl_render_funcs.h"

#include <glm/gtc/type_ptr.hpp>
#include "filesystem.hpp"

namespace gl_code {

    OpenglVars *global_vars;

    void init_vars(OpenglVars &vars) {
        global_vars = &vars;
        vars.mWidth = 800;
        vars.mHeight = 800;
        vars.mNear = 0.1f;
        vars.mFar = 1000.f;

        vars.NUM_LIGHTS = 3;
        vars.NUM_AMB_PROBES = 30;

        vars.ambProbes = std::vector<glm::vec3>();

        vars.activeLights = 0;
        vars.lightMoveMode = false;
        vars.lightMoveInd = 0;

        vars.testCoord = glm::vec3(-10, 12, -3);

        vars.wPressed = false;
        vars.sPressed = false;
        vars.dPressed = false;
        vars.aPressed = false;

        vars.shadowFar = 100.0f;
        vars.shadowNear = 1.0f;

        vars.useTextures = true;

        vars.camera = Camera(glm::vec3(0.f, 0.f, 2.f));

        vars.rands = std::uniform_real_distribution<GLfloat>(0, 1);
        vars.eng = std::default_random_engine(time(NULL));

        vars.renderMode = 0;

        vars.shadowTexNames = {
                "shadowMap0",
                "shadowMap1",
                "shadowMap2"
        };

        vars.cubeVAO = 0;
        vars.cubeVBO = 0;
        vars.cubeTex = 0;

        vars.speed = 6;
        vars.lastTime = -1.0;
        vars.lastX = 400;
        vars.lastY = 300;
        vars.firstMouse = true;
    }

    int gl_main_func(OpenglVars &vars) {
        // Load GLFW and Create a Window
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        auto mWindow = glfwCreateWindow(vars.mWidth, vars.mHeight, "OpenGL", nullptr, nullptr);

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
             << "\n(I removed the click-and-drag rotation)"
             << "\n\n\t- Press L to switch to light movement mode"
             << "\n\t\tWhile in this mode, press SPACE to switch lights"
             << "\n\t\tWASD will move the lights according to the player's orientation"
             << "\n\n\t- Press M to switch between SSAO and full rendering mode"
             << "\n\nI did not implement SSDO because I ran into a bug with GLSL"
             << "\n(seriously, I can demonstrate it), and decided enough was enough." << endl;

        // Set callback functions
        glfwSetKeyCallback(mWindow, key_callback);
        glfwSetCursorPosCallback(mWindow, mouse_callback);
        glfwSetScrollCallback(mWindow, scroll_callback);
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        float am = .05f;

        vars.lights.push_back(
                Light(glm::vec3(-30, 30, -3), glm::vec3(am, am, am), glm::vec3(1, .1, .1), glm::vec3(1, .1, .1), 0));
        vars.lights.push_back(
                Light(glm::vec3(0, 10, 3), glm::vec3(am, am, am), glm::vec3(.1, .1, 1), glm::vec3(.1, .1, 1), 1));
        vars.lights.push_back(
                Light(glm::vec3(30, 30, -3), glm::vec3(am, am, am), glm::vec3(.1, 1, .1), glm::vec3(.1, 1, .1), 2));
        vars.activeLights = 3;

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_DEPTH_TEST);

        // Create a sample shader that displays normal
        vars.fullShader = Shader(FileSystem::getPath("Shaders/geometry.vert.glsl").c_str(),
                                 FileSystem::getPath("Shaders/geometry.frag.glsl").c_str());
        vars.shadowShader = Shader(FileSystem::getPath("Shaders/shadow_geometry.vert.glsl").c_str(),
                                   FileSystem::getPath("Shaders/shadow_geometry.frag.glsl").c_str(),
                                   FileSystem::getPath("Shaders/shadow_geometry.geom.glsl").c_str());
        vars.ssaoShader = Shader(FileSystem::getPath("Shaders/geometry.vert.glsl").c_str(),
                                 FileSystem::getPath("Shaders/ssao_geometry.frag.glsl").c_str());
        vars.ssdoShader = Shader(FileSystem::getPath("Shaders/geometry.vert.glsl").c_str(),
                                 FileSystem::getPath("Shaders/ssdo_geometry.frag.glsl").c_str());
        vars.depthShader = Shader(FileSystem::getPath("Shaders/pre_ssao_geometry.vert.glsl").c_str(),
                                  FileSystem::getPath("Shaders/pre_ssao_geometry.frag.glsl").c_str());
        /*mapShader = Shader(FileSystem::getPath("Shaders/map_geometry.vert.glsl").c_str(),
            FileSystem::getPath("Shaders/map_geometry.frag.glsl").c_str(),
            FileSystem::getPath("Shaders/shadow_geometry.geom.glsl").c_str());*/

        // Load a model from obj file
        vars.sampleModel = Model(FileSystem::getPath("Resources/crytek_sponza/sponza.obj").c_str());

        vars.fullShader.Use();
        for (int i = 0; i < vars.activeLights; i++) {
            vars.lights[i].update(vars.fullShader);
        }

        for (int i = 0; i < vars.NUM_LIGHTS; i++) {
            glGenTextures(1, &vars.shadowMaps[i]);
            glGenFramebuffers(1, &vars.shadowFrameBuffers[i]);
        }

        setupAmbProbes(vars);

        glGenTextures(1, &vars.ambPositionTex);
        glBindTexture(GL_TEXTURE_2D, vars.ambPositionTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F,
                     vars.mWidth, vars.mHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glGenFramebuffers(1, &vars.ambPositionBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, vars.ambPositionBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vars.ambPositionTex, 0);
        GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, attachments);
        glGenRenderbuffers(1, &vars.ambPositionRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, vars.ambPositionRenderBuffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vars.mWidth, vars.mHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, vars.ambPositionRenderBuffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "GBuffer Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //renderMainMap();

        Shader *currentShader;
        // Rendering Loop
        while (glfwWindowShouldClose(mWindow) == false) {
            glfwPollEvents();
            updatePosition(vars);
            // Background Fill Color
            glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (vars.renderMode == 0) {
                currentShader = &vars.fullShader;
                renderShadowMaps(vars);
                renderDepthTex(vars);
                currentShader->Use();
                glUniform1f(glGetUniformLocation(currentShader->Program, "farClip"), vars.shadowFar);
            } else if (vars.renderMode == 1) {
                currentShader = &vars.ssaoShader;
                renderDepthTex(vars);
                currentShader->Use();
            }
            /*
            else {
                currentShader = &ssdoShader;
                currentShader->Use();
            }
            */

            glUniform1i(glGetUniformLocation(currentShader->Program, "positionTex"), 5);
            glActiveTexture(GL_TEXTURE0 + 5);
            glBindTexture(GL_TEXTURE_2D, vars.ambPositionTex);

            /*
            glActiveTexture(GL_TEXTURE0 + 8);
            glUniform1i(glGetUniformLocation(currentShader->Program, "mainMapTex"), 8);
            glBindTexture(GL_TEXTURE_CUBE_MAP, mainMapTex);
            */

            for (int i = 0; i < vars.NUM_AMB_PROBES; i++) {
                glUniform3fv(glGetUniformLocation(currentShader->Program, ("probes[" + to_string(i) + "]").c_str()), 1,
                             glm::value_ptr(vars.ambProbes[i]));
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = glm::perspective(glm::radians(vars.camera.Zoom),
                                                    (GLfloat) vars.mWidth / (GLfloat) vars.mHeight, vars.mNear,
                                                    vars.mFar);
            glm::mat4 view = vars.camera.GetViewMatrix();
            glm::mat4 model = glm::mat4();
            glUniformMatrix4fv(glGetUniformLocation(currentShader->Program, "projection"), 1, GL_FALSE,
                               glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(currentShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniform3fv(glGetUniformLocation(currentShader->Program, "pPos"), 1, glm::value_ptr(vars.camera.Position));

            if (vars.renderMode == 0) {
                for (int i = 0; i < vars.NUM_LIGHTS; i++) {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glUniform1i(glGetUniformLocation(currentShader->Program, vars.shadowTexNames[i]), i);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, vars.shadowMaps[i]);
                }
            }

            vars.useTextures = (vars.renderMode == 0);
            drawMainScene(currentShader, vars);

            model = glm::mat4();
            for (int i = 0; i < vars.activeLights; i++) {
                glUniformMatrix4fv(glGetUniformLocation(currentShader->Program, "model"),
                                   1,
                                   GL_FALSE,
                                   glm::value_ptr(glm::translate(model, vars.lights[i].getPos())));
                RenderCube(*currentShader, vars);
            }

            // Flip Buffers and Draw
            glfwSwapBuffers(mWindow);
        }
        glfwTerminate();
        return EXIT_SUCCESS;
    }

    void renderMainMap(OpenglVars &vars) {
        vars.mapShader.Use();
        float mapRes = 100.0f;
        glViewport(0, 0, mapRes, mapRes);
        glm::mat4 perspective = glm::perspective(glm::radians(90.0f), 1.0f, vars.mNear, vars.mFar);
        glm::vec3 center(0, 10, 0);
        glUniformMatrix4fv(glGetUniformLocation(vars.mapShader.Program, "shadowMat[0]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(center,
                                                             center + glm::vec3(1, 0, 0),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.mapShader.Program, "shadowMat[1]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(center,
                                                             center + glm::vec3(-1, 0, 0),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.mapShader.Program, "shadowMat[2]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(center,
                                                             center + glm::vec3(0, 1, 0),
                                                             glm::vec3(0, 0, 1))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.mapShader.Program, "shadowMat[3]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(center,
                                                             center + glm::vec3(0, -1, 0),
                                                             glm::vec3(0, 0, -1))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.mapShader.Program, "shadowMat[4]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(center,
                                                             center + glm::vec3(0, 0, 1),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.mapShader.Program, "shadowMat[5]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(center,
                                                             center + glm::vec3(0, 0, -1),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniform3fv(glGetUniformLocation(vars.shadowShader.Program, "lightPos"), 1, glm::value_ptr(center));
        glUniform1f(glGetUniformLocation(vars.shadowShader.Program, "farClip"), vars.shadowFar);

        glGenTextures(1, &vars.mainMapTex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, vars.mainMapTex);
        for (GLuint i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                         mapRes, mapRes, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        GLuint mapFB;
        glGenFramebuffers(1, &mapFB);
        glBindFramebuffer(GL_FRAMEBUFFER, mapFB);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, vars.mainMapTex, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glClear(GL_DEPTH_BUFFER_BIT);

        vars.useTextures = true;
        drawMainScene(&vars.mapShader, vars);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
        glViewport(0, 0, vars.mWidth, vars.mHeight);
    }

    void setupAmbProbes(OpenglVars &vars) {
        for (GLuint i = 0; i < vars.NUM_AMB_PROBES; i++) {
            vars.ambProbes.push_back(glm::vec3(
                    vars.rands(vars.eng) * 2 - 1,
                    vars.rands(vars.eng) * 2 - 1,
                    vars.rands(vars.eng)
            ));
            vars.ambProbes[i] = glm::normalize(vars.ambProbes[i]);
            GLfloat ratio = ((GLfloat) i) / vars.NUM_AMB_PROBES;
            vars.ambProbes[i] *= .1 + .9 * ratio * ratio;
        }
    }

    void renderDepthTex(OpenglVars &vars) {
        glm::mat4 projection = glm::perspective(glm::radians(vars.camera.Zoom),
                                                (GLfloat) vars.mWidth / (GLfloat) vars.mHeight, vars.mNear,
                                                vars.mFar);
        glm::mat4 view = vars.camera.GetViewMatrix();
        glm::mat4 model = glm::mat4();
        vars.depthShader.Use();
        glViewport(0, 0, vars.mWidth, vars.mHeight);
        glUniformMatrix4fv(glGetUniformLocation(vars.depthShader.Program, "projection"), 1, GL_FALSE,
                           glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(vars.depthShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform1f(glGetUniformLocation(vars.depthShader.Program, "farClip"), vars.mFar);

        glBindFramebuffer(GL_FRAMEBUFFER, vars.ambPositionBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vars.useTextures = false;
        drawMainScene(&vars.depthShader, vars);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        vars.useTextures = true;
    }

    void setupShadowMatrices(int lightIndex, OpenglVars &vars) {
        glm::mat4 perspective = glm::perspective(glm::radians(90.0f), 1.0f, vars.shadowNear, vars.shadowFar);
        glUniformMatrix4fv(glGetUniformLocation(vars.shadowShader.Program, "shadowMat[0]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(vars.lights[lightIndex].getPos(),
                                                             vars.lights[lightIndex].getPos() + glm::vec3(1, 0, 0),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.shadowShader.Program, "shadowMat[1]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(vars.lights[lightIndex].getPos(),
                                                             vars.lights[lightIndex].getPos() + glm::vec3(-1, 0, 0),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.shadowShader.Program, "shadowMat[2]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(vars.lights[lightIndex].getPos(),
                                                             vars.lights[lightIndex].getPos() + glm::vec3(0, 1, 0),
                                                             glm::vec3(0, 0, 1))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.shadowShader.Program, "shadowMat[3]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(vars.lights[lightIndex].getPos(),
                                                             vars.lights[lightIndex].getPos() + glm::vec3(0, -1, 0),
                                                             glm::vec3(0, 0, -1))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.shadowShader.Program, "shadowMat[4]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(vars.lights[lightIndex].getPos(),
                                                             vars.lights[lightIndex].getPos() + glm::vec3(0, 0, 1),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniformMatrix4fv(glGetUniformLocation(vars.shadowShader.Program, "shadowMat[5]"), 1, GL_FALSE,
                           glm::value_ptr(
                                   perspective * glm::lookAt(vars.lights[lightIndex].getPos(),
                                                             vars.lights[lightIndex].getPos() + glm::vec3(0, 0, -1),
                                                             glm::vec3(0, -1, 0))
                           ));
        glUniform3fv(glGetUniformLocation(vars.shadowShader.Program, "lightPos"), 1,
                     glm::value_ptr(vars.lights[lightIndex].getPos()));
        glUniform1f(glGetUniformLocation(vars.shadowShader.Program, "farClip"), vars.shadowFar);
    }

    void renderShadowMaps(OpenglVars &vars) {
        vars.useTextures = false;
        vars.shadowShader.Use();
        GLuint shadowRes = 1024;
        glViewport(0, 0, shadowRes, shadowRes);

        for (int lightNum = 0; lightNum < vars.NUM_LIGHTS; lightNum++) {
            setupShadowMatrices(lightNum, vars);
            glBindTexture(GL_TEXTURE_CUBE_MAP, vars.shadowMaps[lightNum]);
            for (GLuint i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                             shadowRes, shadowRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, vars.shadowFrameBuffers[lightNum]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, vars.shadowMaps[lightNum], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glClear(GL_DEPTH_BUFFER_BIT);

            drawMainScene(&vars.shadowShader, vars);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
        glViewport(0, 0, vars.mWidth, vars.mHeight);
        vars.useTextures = true;
    }

    void drawMainScene(Shader *currentShader, OpenglVars &vars) {
        glm::mat4 model;
        model = glm::scale(model, glm::vec3(0.05f));    // The sponza model is too big, scale it first
        glUniformMatrix4fv(glGetUniformLocation(currentShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        vars.sampleModel.Draw(*currentShader, vars);
    }

    // RenderCube() Renders a 1x1 3D cube in NDC.
    void RenderCube(const Shader &shader, OpenglVars &vars) {
        // Initialize (if necessary)
        if (vars.cubeVAO == 0) {
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
            glGenVertexArrays(1, &vars.cubeVAO);
            glGenBuffers(1, &vars.cubeVBO);
            // Fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, vars.cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // Link vertex attributes
            glBindVertexArray(vars.cubeVAO);
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
            glGenTextures(1, &vars.cubeTex);
            glBindTexture(GL_TEXTURE_2D, vars.cubeTex);
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
        glBindTexture(GL_TEXTURE_2D, vars.cubeTex);
        glBindVertexArray(vars.cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void updatePosition(OpenglVars &vars) {
        if (vars.lastTime == -1.0) {
            vars.lastTime = glfwGetTime();
            return;
        }

        double deltaTime = glfwGetTime() - vars.lastTime;
        vars.lastTime = glfwGetTime();

        if (vars.lightMoveMode) {
            if (vars.wPressed) {
                vars.lights[vars.lightMoveInd].ProcessKeyboard(FORWARD, vars.speed * deltaTime, vars.camera.Front,
                                                               vars.camera.Right);
            }
            if (vars.sPressed) {
                vars.lights[vars.lightMoveInd].ProcessKeyboard(BACKWARD, vars.speed * deltaTime, vars.camera.Front,
                                                               vars.camera.Right);
            }
            if (vars.aPressed) {
                vars.lights[vars.lightMoveInd].ProcessKeyboard(LEFT, vars.speed * deltaTime, vars.camera.Front,
                                                               vars.camera.Right);
            }
            if (vars.dPressed) {
                vars.lights[vars.lightMoveInd].ProcessKeyboard(RIGHT, vars.speed * deltaTime, vars.camera.Front,
                                                               vars.camera.Right);
            }
            vars.lights[vars.lightMoveInd].update(vars.fullShader);
        } else {
            if (vars.wPressed) {
                vars.camera.ProcessKeyboard(FORWARD, vars.speed * deltaTime);
            }
            if (vars.sPressed) {
                vars.camera.ProcessKeyboard(BACKWARD, vars.speed * deltaTime);
            }
            if (vars.aPressed) {
                vars.camera.ProcessKeyboard(LEFT, vars.speed * deltaTime);
            }
            if (vars.dPressed) {
                vars.camera.ProcessKeyboard(RIGHT, vars.speed * deltaTime);
            }
        }
    }

// Is called whenever a key is pressed/released via GLFW
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        // Camera movements
        switch (key) {
            case GLFW_KEY_W:
                global_vars->wPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_S:
                global_vars->sPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_A:
                global_vars->aPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_D:
                global_vars->dPressed = action != GLFW_RELEASE;
                break;
            case GLFW_KEY_L:
                if (action == GLFW_PRESS) {
                    global_vars->lightMoveMode = !global_vars->lightMoveMode;
                }
                break;
            case GLFW_KEY_M:
                if (action == GLFW_PRESS) {
                    global_vars->renderMode = (global_vars->renderMode + 1) % 2;
                }
                break;
            case GLFW_KEY_SPACE:
                if (global_vars->lightMoveMode && action == GLFW_PRESS) {
                    global_vars->lightMoveInd = (global_vars->lightMoveInd + 1) % global_vars->activeLights;
                }
                break;
        }
    }

    void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        //TODO: changed
        //if (state == GLFW_PRESS) {
        if (true) {
            if (global_vars->firstMouse) {
                global_vars->lastX = xpos;
                global_vars->lastY = ypos;
                global_vars->firstMouse = false;
            }

            GLfloat xoffset = xpos - global_vars->lastX;
            GLfloat yoffset = global_vars->lastY - ypos;

            global_vars->lastX = xpos;
            global_vars->lastY = ypos;
            global_vars->camera.ProcessMouseMovement(xoffset, yoffset);
        }
        /*if (state == GLFW_RELEASE) {
            firstMouse = true;
        }*/
    }

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        global_vars->camera.ProcessMouseScroll(yoffset);
    }

}  // namespace gl_code