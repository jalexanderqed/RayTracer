#include "ray_tracer.h"

static void loadScene(char *name) {
	/* load the scene into the SceneIO data structure using given parsing code */
	scene = readScene(name);
	if (scene == NULL) exit(1);

	// Calculates a custom epsilon value based on size of the scene
	BoundingBox sceneBox = boundScene(scene);
	float diffBound = max(sceneBox.vMax.x - sceneBox.vMin.x, 
		max(sceneBox.vMax.y - sceneBox.vMin.y, sceneBox.vMax.z - sceneBox.vMin.z)) / 2.0f;	
	float posBound = max(sceneBox.vMax.x,
		max(sceneBox.vMax.y, sceneBox.vMax.z));
	float negBound = min(sceneBox.vMin.x,
		min(sceneBox.vMin.y, sceneBox.vMin.z));
	float largeBound = max(abs(posBound), max(abs(negBound), diffBound));

	// EPS_FACTOR was determined with testing of different epsilon values on scenes
	EPSILON = min(largeBound / EPS_FACTOR, 0.0001f);

	if (SAMPLES_PER_PIXEL != 1) {
		int len = strlen(name);
		globalFocalDistance = scene->camera->focalDistance;
		switch (name[len - 7]) {
		case '3':
			lensSide = 50;
			break;
		case '5':
			lensSide = 100;
			break;
		}
	}

	if (complexIntersectShaders && complexColorShaders) {
		int count = 0;
		for (ObjIO *object = scene->objects; object != NULL; object = object->next) {
			if (object->type == SPHERE_OBJ) {
				sphereMap.insert({ object, count });
				count++;
			}
		}
	}

	// Builds accelleration structures
	jacksBuildBounds(scene);

	return;
}


/* just a place holder, feel free to edit */
void render(void) {
	image = new float[sizeof(float) * IMAGE_HEIGHT * IMAGE_WIDTH * 3];

	jacksRenderScene(scene);
/*
	CImage imageObj;
	imageObj.Create(IMAGE_WIDTH, IMAGE_HEIGHT, 24);
	BYTE* imagePtr = (BYTE*)imageObj.GetBits();
	int pitch = imageObj.GetPitch();

	for (int j = 0; j < IMAGE_HEIGHT; j++) {
		for (int i = 0; i < IMAGE_WIDTH; i++) {
			glm::vec3 pixel = getPixel(i, IMAGE_HEIGHT - j - 1);
			*(imagePtr + pitch * j + i * 3 + 0) = (BYTE)(min(pixel.b * 255, 255.0f));
			*(imagePtr + pitch * j + i * 3 + 1) = (BYTE)(min(pixel.g * 255, 255.0f));
			*(imagePtr + pitch * j + i * 3 + 2) = (BYTE)(min(pixel.r * 255, 255.0f));
		}
	}

	imageObj.Save(fileName);

	// Removed because it interferes with reading times
	//wcout << "Image saved to " << fileName << endl;

	imageObj.Destroy();
	delete[] image;
*/
	return;
}

inline void cross(const glm::vec3& v1, float* v2, float* res) {
	res[0] = v1[1] * v2[2] - v1[2] * v2[1];
	res[1] = v1[2] * v2[0] - v1[0] * v2[2];
	res[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
