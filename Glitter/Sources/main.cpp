// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "ray_tracer.h"
#include "Tracer.h"
#include "SceneStructure.h"
#include "Timer.h"

int main(int argc, char *argv[]) {
    RayTracerParams scene_params;
    scene_params.IMAGE_WIDTH = 1500;
    scene_params.IMAGE_HEIGHT = 1500;
    scene_params.EPSILON = 0.00005f;
    scene_params.EPS_FACTOR = 67000;
    scene_params.numThreads = 8;
    scene_params.useAcceleration = true;
    scene_params.complexColorShaders = false;
    scene_params.complexIntersectShaders = false;
    scene_params.SAMPLES_PER_PIXEL = 1;
    scene_params.lensSide = 5.0f;
    scene_params.focalLength = 0.01f;
    scene_params.globalFocalDistance = 2;

    SceneData scene_data;

    srand(time(NULL));
    Timer total_timer;
    Timer accelTimer;
    Timer renderTimer;
    total_timer.startTimer();

    if (argc < 2) {
        cout << "Usage:\tBasicRayTracer.exe <input file> [<output file>]" << endl;
        exit(1);
    }
/*
	if (argc >= 3) {
		fileName = CA2W(argv[2]);
	}
	else {
		fileName = _T("output.png");
	}

	if (complexColorShaders || complexIntersectShaders) {
		HRESULT success = texture1.Load(CA2W("earth.jpg"));
		if (success != S_OK) {
			cerr << "Error loading texture 1: " << success << endl;
			exit(1);
		}
	}
 */

    scene_data.sceneName = argv[1];
    accelTimer.startTimer();
    loadScene(scene_data);
    updateRayTracerParams(scene_data, scene_params);
    BuildAccelerators(scene_data, scene_params);
    accelTimer.stopTimer();
    fprintf(stderr, "Scene-building time: %.5lf secs\n", accelTimer.getTime());

    renderTimer.startTimer();
    render(scene_data, scene_params);
    renderTimer.stopTimer();
    fprintf(stderr, "Rendering time: %.5lf secs\n", renderTimer.getTime());

    if (scene_data.sceneGeometry != NULL) {
        deleteScene(scene_data.sceneGeometry);
    }

    jacksCleanupBounds(scene_data);

    total_timer.stopTimer();
    fprintf(stderr, "Total time: %.5lf secs\n\n", total_timer.getTime());

    if (true) return 0;

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

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
