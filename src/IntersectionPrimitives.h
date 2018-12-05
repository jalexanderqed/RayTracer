#ifndef INTERSECTION_PRIMITIVES_H
#define INTERSECTION_PRIMITIVES_H

#define _USE_MATH_DEFINES
#define GLM_EXT_INCLUDED
#define GLM_ENABLE_EXPERIMENTAL

#include <cmath>
#include <algorithm>
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "sphere.h"
#include "tracer_structs.h"

using namespace std;

glm::vec3 getBarycentricWeights(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3,
                                const glm::vec3 &point);

glm::vec3 interpolateVecs(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3,
                          const glm::vec3 &inter1, const glm::vec3 &inter2, const glm::vec3 &inter3,
                          const glm::vec3 &point);

class PolyIntersectionPoint {
 public:
  glm::vec3 position;
  const PolygonIO *poly;

  PolyIntersectionPoint();

  PolyIntersectionPoint() = default;

  PolyIntersectionPoint(const glm::vec3 &p, const Mesh& py);
};

class IntersectionPoint {
 public:
  glm::vec3 position;
  const GeoObject& object;
  const PolyIntersectionPoint& polyIntersect;

  IntersectionPoint();

  IntersectionPoint(const glm::vec3& p,
		    const GeoObject& o,
		    const PolyIntersectionPoint& pi);
};

void calcUVPoly(const PolyIntersectionPoint &iPoint, float &u, float &v);

void calcUVSphere(const IntersectionPoint &iPoint, float &u, float &v);

IntersectionPoint intersectSphere(const glm::vec3 &vec,
                                  const glm::vec3 &origin,
                                  const Sphere& sphere,
                                  SceneData scene_data,
                                  RayTracerParams scene_params);

PolyIntersectionPoint intersectPoly(const glm::vec3 &vec,
                                    const glm::vec3 &origin,
                                    const Mesh& poly,
                                    RayTracerParams scene_params);

IntersectionPoint intersectPolySet(const glm::vec3 &vec,
                                   const glm::vec3 &origin,
                                   const ObjIO *shape,
                                   RayTracerParams scene_params);

#endif // !INTERSECTION_PRIMITIVES_H
