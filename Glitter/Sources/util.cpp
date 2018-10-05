#include "util.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void setPixel(int x, int y, const glm::vec3& color, SceneData scene_data, RayTracerParams scene_params) {
	*(scene_data.image + 3 * scene_params.IMAGE_WIDTH * y + x * 3 + 0) = color.r;
	*(scene_data.image + 3 * scene_params.IMAGE_WIDTH * y + x * 3 + 1) = color.g;
	*(scene_data.image + 3 * scene_params.IMAGE_WIDTH * y + x * 3 + 2) = color.b;
}

glm::vec3 getPixel(int x, int y, SceneData scene_data, RayTracerParams scene_params) {
	return glm::vec3(*(scene_data.image + 3 * scene_params.IMAGE_WIDTH * y + x * 3 + 0),
		*(scene_data.image + 3 * scene_params.IMAGE_WIDTH * y + x * 3 + 1),
		*(scene_data.image + 3 * scene_params.IMAGE_WIDTH * y + x * 3 + 2));
}

glm::vec3 reflect(const glm::vec3& out, const glm::vec3& normal) {
	return glm::normalize(2 * glm::dot(normal, out) * normal - out);
}

glm::vec3 refract(const glm::vec3& out, const glm::vec3& normal, float oldIR, float newIR) {
	float dotProd = glm::dot(out, normal);
	glm::vec3 outNormal = dotProd < 0 ? -1.0f * normal : normal;

	glm::vec3 in = -1.0f * out;
	float ratio = (oldIR / newIR);
	float cosine = glm::dot(outNormal, in);
	float sin2 = ratio * ratio * (1.0f - cosine * cosine);
	if (sin2 > 1) return glm::vec3(0);
	return glm::normalize(ratio * in - (ratio * cosine + sqrt(1.0f - sin2)) * outNormal);
}

int write_img(char* file_name, SceneData scene_data, RayTracerParams scene_params){
    unsigned char* img_ptr = new unsigned char[scene_params.IMAGE_WIDTH * scene_params.IMAGE_HEIGHT * 3];
    for (int y = 0; y < scene_params.IMAGE_HEIGHT; y++) {
        for (int x = 0; x < scene_params.IMAGE_WIDTH; x++) {
            glm::vec3 pixel = getPixel(x, scene_params.IMAGE_HEIGHT - y - 1, scene_data, scene_params);
            *(img_ptr + scene_params.IMAGE_WIDTH * y * 3 + x * 3 + 0) = (unsigned char)(max(min(pixel.r * 255, 255.0f), 0.0f));
            *(img_ptr + scene_params.IMAGE_WIDTH * y * 3 + x * 3 + 1) = (unsigned char)(max(min(pixel.g * 255, 255.0f), 0.0f));
            *(img_ptr + scene_params.IMAGE_WIDTH * y * 3 + x * 3 + 2) = (unsigned char)(max(min(pixel.b * 255, 255.0f), 0.0f));
        }
    }
    int x = 250;
    int y = 250;
    glm::vec3 pixel = getPixel(x, scene_params.IMAGE_HEIGHT - y - 1, scene_data, scene_params);
    cout << pixel.r << ", " << pixel.g << ", " << pixel.b << endl;

    return write_img(file_name, img_ptr, scene_params.IMAGE_WIDTH, scene_params.IMAGE_HEIGHT);
}

int write_img(char* file_name, unsigned char* image, int width, int height){
	return stbi_write_bmp(file_name, width, height, 3, image);
}