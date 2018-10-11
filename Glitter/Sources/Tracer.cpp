#include "Tracer.h"

#define CLIP_DIST 100

#define LIGHT_NUM 1.0f
#define LIGHT_C1 0.25f
#define LIGHT_C2 0.1f
#define LIGHT_C3 0.01f

#define SHINY_MULT 40

//#define DEBUG

using shared_obj::Light;

const glm::vec3 BACK_COLOR(0);

vector<Light> lights;

MaterialIO getComplexMaterial(const IntersectionPoint &iPoint, SceneData &scene_data) {
    if (iPoint.object->type == SPHERE_OBJ) {

        float u, v;
        calcUVSphere(iPoint, u, v);

        v = 1 - v;
        if (v >= 1) v = 0.999f;

        if (scene_data.sphereMap.count(iPoint.object) > 0) {
            float sphereInd = scene_data.sphereMap.at(iPoint.object);
            float rotate = sphereInd / scene_data.sphereMap.size();
            if (u >= 1.0f - rotate) u += -1 + rotate;
            else u += rotate;
        }

        /*
        COLORREF color = texture1.GetPixel((int)(u * texture1.GetWidth()), (int)(v * texture1.GetHeight()));
        if (color == CLR_INVALID) {
            cerr << "Could not load color value at " <<
                (int)(u * texture1.GetWidth()) << ", " <<
                (int)(v * texture1.GetHeight()) << endl;
            cerr << "u: " << u << endl;
            cerr << "v: " << v << endl;
            exit(1);
        }

        glm::vec3 vecColor(GetRValue(color) / 255.0f, GetGValue(color) / 255.0f, GetBValue(color) / 255.0f);
         */
        glm::vec3 vecColor(0);
        MaterialIO mat = dupMaterial(iPoint.object->material);
        float diffColor = (glm::normalize(vecColor).b > 0.5f) ? 0.0f : 1.0f;
        if (scene_data.sphereMap.count(iPoint.object) > 0) {
            switch (scene_data.sphereMap.at(iPoint.object)) {
                case 0:
                case 1:
                    mat.shininess = (diffColor + 0.25f) / 2;
                    mat.diffColor[0] = mat.diffColor[1] = mat.diffColor[2] = diffColor;
                    mat.specColor[0] = mat.specColor[1] = mat.specColor[2] = (diffColor + 0.25f) / 2;
                    break;
                case 3:
                    mat.ktran = (diffColor + 0.8f) / 2;
                case 2:
                case 4:
                    mat.diffColor[0] = vecColor.r;
                    mat.diffColor[1] = vecColor.g;
                    mat.diffColor[2] = vecColor.b;
                    break;
                default:

                    break;
            }
        }

        return mat;
    } else {
        float u, v;
        calcUVPoly(iPoint.polyIntersect, u, v);

        float color = (u + v) / 4;

        MaterialIO mat = dupMaterial(iPoint.object->material);
        mat.diffColor[0] = mat.diffColor[1] = mat.diffColor[2] = color;
        return mat;
    }
}

MaterialIO getMaterial(const IntersectionPoint &iPoint, SceneData &scene_data, RayTracerParams &scene_params) {
    if (scene_params.complexColorShaders) {
        return getComplexMaterial(iPoint, scene_data);
    }

    if (iPoint.object->type == POLYSET_OBJ &&
        iPoint.object->numMaterials > 1 &&
        ((PolySetIO *) (iPoint.object->data))->materialBinding == PER_VERTEX_MATERIAL) {
        glm::vec3 p1(iPoint.polyIntersect.poly->vert[0].pos[0],
                     iPoint.polyIntersect.poly->vert[0].pos[1],
                     iPoint.polyIntersect.poly->vert[0].pos[2]);
        glm::vec3 p2(iPoint.polyIntersect.poly->vert[1].pos[0],
                     iPoint.polyIntersect.poly->vert[1].pos[1],
                     iPoint.polyIntersect.poly->vert[1].pos[2]);
        glm::vec3 p3(iPoint.polyIntersect.poly->vert[2].pos[0],
                     iPoint.polyIntersect.poly->vert[2].pos[1],
                     iPoint.polyIntersect.poly->vert[2].pos[2]);
        return interpolateMaterials(p1, p2, p3,
                                    &iPoint.object->material[iPoint.polyIntersect.poly->vert[0].materialIndex],
                                    &iPoint.object->material[iPoint.polyIntersect.poly->vert[1].materialIndex],
                                    &iPoint.object->material[iPoint.polyIntersect.poly->vert[2].materialIndex],
                                    iPoint.position);
    } else {
        return *(iPoint.object->material);
    }
}

glm::vec3 lightContrib(const glm::vec3 &lightColor,
                       const glm::vec3 &normal, const glm::vec3 &inDir,
                       const glm::vec3 &dirToLight, float distToLight,
                       const glm::vec3 &diffuseColor, const glm::vec3 &specularColor,
                       float shiny, float kTrans) {
    glm::vec3 contrib(0);
    glm::vec3();
    glm::vec3 outDir = inDir * -1.0f;
    float diffuseContrib = max(0.0f, glm::dot(normal, dirToLight));

    contrib += (1.0f - kTrans) * diffuseContrib * diffuseColor;

    glm::vec3 lightReflectDir = reflect(dirToLight, normal);
    float specularContrib = max(0.0f, pow(glm::dot(lightReflectDir, outDir), shiny * SHINY_MULT));
    contrib += specularContrib * specularColor;

    float atten = distToLight > 0 ?
                  min(1.0f, LIGHT_NUM / (LIGHT_C1 + LIGHT_C2 * distToLight + LIGHT_C3 * distToLight * distToLight))
                                  : 1.0f;

    glm::vec3 color = atten * contrib * lightColor;
    return color;
}

glm::vec3 calcAllLights(const IntersectionPoint &iPoint,
                        const MaterialIO &interMaterial,
                        const glm::vec3 &inVec,
                        SceneData &scene_data,
                        RayTracerParams &scene_params) {

    glm::vec3 normal = getNormal(iPoint);
    glm::vec3 diffuse(interMaterial.diffColor[0], interMaterial.diffColor[1], interMaterial.diffColor[2]);
    glm::vec3 specular(interMaterial.specColor[0], interMaterial.specColor[1], interMaterial.specColor[2]);
    glm::vec3 ambient(interMaterial.ambColor[0], interMaterial.ambColor[1], interMaterial.ambColor[2]);
    glm::vec3 emissive(interMaterial.emissColor[0], interMaterial.emissColor[1], interMaterial.emissColor[2]);
    float shiny = interMaterial.shininess;
    float trans = interMaterial.shininess;

    glm::vec3 color = ambient * diffuse * (1.0f - interMaterial.ktran);

    for (Light l : lights) {
        glm::vec3 dirToLight(0);
        float dist2;

        switch (l.light_type()) {
            case SPOT_LIGHT:
            case POINT_LIGHT:
                dirToLight = l.position() - iPoint.position;
                dist2 = glm::length2(dirToLight);
                break;
            case DIRECTIONAL_LIGHT:
                dirToLight = -1.0f * l.direction();
                break;
        }

        dirToLight = glm::normalize(dirToLight);

        IntersectionPoint ip = intersectScene(dirToLight,
                                              iPoint.position + scene_params.EPSILON * dirToLight,
                                              scene_data,
                                              scene_params);
        MaterialIO ipMaterial;
        glm::vec3 seenColor = l.color();
        while (!(ip.object == NULL ||
                 (ipMaterial = getMaterial(ip, scene_data, scene_params)).ktran < scene_params.EPSILON ||
                 (l.light_type() != DIRECTIONAL_LIGHT &&
                  glm::distance2(ip.position, l.position()) > dist2))) {
            glm::vec3 objColor(ipMaterial.diffColor[0],
                               ipMaterial.diffColor[1],
                               ipMaterial.diffColor[2]);
            objColor *= 1.0f / max(objColor.r, max(objColor.g, objColor.b));
            seenColor = seenColor * ipMaterial.ktran * objColor;

            ip = intersectScene(dirToLight,
                                ip.position + scene_params.EPSILON * dirToLight,
                                scene_data,
                                scene_params);
        }

        if (ip.object == NULL ||
            (l.light_type() != DIRECTIONAL_LIGHT &&
             glm::distance2(ip.position, l.position()) > dist2) ||
            ipMaterial.ktran > scene_params.EPSILON) {
            glm::vec3 light_contrib = lightContrib(seenColor, normal, inVec, dirToLight,
                                                   l.light_type() == DIRECTIONAL_LIGHT ? -1 : glm::distance(
                                                           l.position(), ip.position),
                                                   diffuse, specular, shiny, interMaterial.ktran);
            color += light_contrib;
        }
    }
    return color;
}

glm::vec3 shadeIntersect(const IntersectionPoint &iPoint,
                         const glm::vec3 &inVec,
                         int inside,
                         int depth,
                         SceneData &scene_data,
                         RayTracerParams &scene_params) {
    glm::vec3 outVec = -1.0f * inVec;
    MaterialIO interMaterial = getMaterial(iPoint, scene_data, scene_params);

    glm::vec3 color = calcAllLights(iPoint, interMaterial, inVec, scene_data, scene_params);
    if (depth > 10) return color;
    depth++;

    glm::vec3 normal = glm::normalize(getNormal(iPoint));

    if (interMaterial.ktran > scene_params.EPSILON) {
        float cosVal = glm::dot(inVec, normal);
        float oldIR, newIR;
        if (cosVal > 0) { // Going out
            inside = max(0, inside - 1);
            oldIR = 1.5f;
            newIR = inside == 0 ? 1.0f : 1.5f;
        } else { // Going in
            oldIR = inside == 0 ? 1.0f : 1.5f;
            inside++;
            newIR = 1.5f;
        }

        glm::vec3 refractDir = oldIR == newIR ? inVec : refract(outVec, normal, oldIR, newIR);
        if (glm::length2(refractDir) < scene_params.EPSILON) { // Total internal reflection
            inside++;
        } else {
            float newCosVal = glm::dot(refractDir, normal);
            IntersectionPoint refractIntersect = intersectScene(refractDir,
                                                                iPoint.position + scene_params.EPSILON * refractDir,
                                                                scene_data,
                                                                scene_params);
            if (refractIntersect.object != NULL) {
                color += interMaterial.ktran *
                         shadeIntersect(refractIntersect, refractDir, inside, depth, scene_data, scene_params);
            }
        }
    }
    if (interMaterial.specColor[0] +
        interMaterial.specColor[1] +
        interMaterial.specColor[2]
        > scene_params.EPSILON) {
        glm::vec3 spec(interMaterial.specColor[0],
                       interMaterial.specColor[1],
                       interMaterial.specColor[2]);

        glm::vec3 reflectDir = reflect(outVec, normal);
        IntersectionPoint reflectIntersect = intersectScene(reflectDir,
                                                            iPoint.position + scene_params.EPSILON * reflectDir,
                                                            scene_data,
                                                            scene_params);
        if (reflectIntersect.object != NULL) {
            glm::vec3 reflect_result = shadeIntersect(
                    reflectIntersect,
                    reflectDir,
                    inside,
                    depth,
                    scene_data,
                    scene_params);
            color += spec * reflect_result;
        }
    }
    return color;
}

glm::vec3 tracePixelVec(
        const glm::vec3 &firstVec,
        const glm::vec3 &camPos,
        SceneData &scene_data,
        RayTracerParams &scene_params
) {
    IntersectionPoint iPoint = intersectScene(firstVec, camPos, scene_data, scene_params);

    if (iPoint.object != NULL) {
        return shadeIntersect(iPoint, firstVec, 0, 0, scene_data, scene_params);
    } else {
        return BACK_COLOR;
    }
}

int renderLoop(SceneData *scene_d, RayTracerParams *scene_p, int threadNum) {
    SceneData &scene_data = *scene_d;
    RayTracerParams &scene_params = *scene_p;
    SceneIO *scene = scene_data.sceneGeometry;
    SceneCamera cam(scene->camera, scene_params);

    thread_local default_random_engine generator(threadNum + time(NULL));
    uniform_real_distribution<float> randomGen(0.0f, 1.0f);

    int lastPercent = 0;
    float invSampPix = 1.0f / scene_params.SAMPLES_PER_PIXEL;
    glm::vec3 focalPlane = cam.screenPos + cam.focalDistance * cam.forward;

    for (int pixY = threadNum; pixY < scene_params.IMAGE_HEIGHT; pixY += scene_params.numThreads) {
        for (int pixX = 0; pixX < scene_params.IMAGE_WIDTH; pixX++) {
            glm::vec3 color(0);
            for (int subY = 0; subY < scene_params.SAMPLES_PER_PIXEL; subY++) {
                for (int subX = 0; subX < scene_params.SAMPLES_PER_PIXEL; subX++) {
                    glm::vec2 screenSpace(0);

                    if (scene_params.SAMPLES_PER_PIXEL == 1) {
                        screenSpace.x = (pixX + 0.5f) / scene_params.IMAGE_WIDTH;
                        screenSpace.y = (pixY + 0.5f) / scene_params.IMAGE_HEIGHT;

                        glm::vec3 screenPoint = cam.screenPos +
                                                (2 * screenSpace.x - 1) * cam.screenHoriz +
                                                (2 * screenSpace.y - 1) * cam.screenVert;

                        glm::vec3 pixVec = glm::normalize(screenPoint - cam.pos);

                        color = color + tracePixelVec(pixVec,
                                                      cam.pos,
                                                      scene_data,
                                                      scene_params)
                                        / ((float) scene_params.SAMPLES_PER_PIXEL
                                           * scene_params.SAMPLES_PER_PIXEL);
                    } else {
                        screenSpace.x =
                                (pixX + ((float) subX / scene_params.SAMPLES_PER_PIXEL) +
                                 randomGen(generator) * invSampPix) /
                                scene_params.IMAGE_WIDTH;
                        screenSpace.y =
                                (pixY + ((float) subY / scene_params.SAMPLES_PER_PIXEL) +
                                 randomGen(generator) * invSampPix) /
                                scene_params.IMAGE_HEIGHT;

                        glm::vec3 screenPoint = cam.screenPos -
                                                (2 * screenSpace.x - 1) * cam.screenHoriz -
                                                (2 * screenSpace.y - 1) * cam.screenVert;

                        glm::vec3 calcRay = glm::normalize(cam.lens - screenPoint);
                        float d = glm::dot(focalPlane - screenPoint, cam.forward) / glm::dot(calcRay, cam.forward);
                        glm::vec3 focalPlanePoint = screenPoint + calcRay * d;

                        glm::vec3 lensPoint = cam.lens +
                                              cam.screenHoriz * (randomGen(generator) * 2 * scene_params.lensSide -
                                                                 scene_params.lensSide) +
                                              cam.screenVert * (randomGen(generator) * 2 * scene_params.lensSide -
                                                                scene_params.lensSide);
                        glm::vec3 pixVec = glm::normalize(focalPlanePoint - lensPoint);

                        color = color + tracePixelVec(pixVec, lensPoint, scene_data, scene_params) /
                                        ((float) scene_params.SAMPLES_PER_PIXEL * scene_params.SAMPLES_PER_PIXEL);
                    }
                }
            }
            setPixel(pixX, pixY, color, scene_data, scene_params);

            if (scene_params.numThreads == 1) {
                float percent = ((float) pixY * scene_params.IMAGE_WIDTH + pixX) /
                                (scene_params.IMAGE_WIDTH * scene_params.IMAGE_HEIGHT);
                if (percent * 100 > lastPercent + 10) {
                    lastPercent += 10;
                    cerr << lastPercent << "% complete" << endl;
                }
            }
        }
    }
    return 0;
}

void jacksRenderScene(SceneData &scene_data, RayTracerParams &scene_params) {
    SceneIO *scene = scene_data.sceneGeometry;
    for (LightIO *light = scene->lights; light != NULL; light = light->next) {
        lights.push_back(Light(light));
    }

    if (scene_params.useAcceleration && scene_params.numThreads > 1) {
        thread *threads = new thread[scene_params.numThreads];

        for (int i = 0; i < scene_params.numThreads; i++) {
            threads[i] = thread(renderLoop, &scene_data, &scene_params, i);
        }

        for (int i = 0; i < scene_params.numThreads; i++) {
            threads[i].join();
        }

        delete[] threads;
    } else {
        scene_params.numThreads = 1;
        renderLoop(&scene_data, &scene_params, 0);
    }
}