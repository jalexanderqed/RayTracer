#ifndef POLY_BOUND_H
#define POLY_BOUND_H

#include "tracer_structs.h"
#include "BoundingBox.h"
#include "IntersectionPrimitives.h"
#include "scene_io.h"
#include <list>
#include <iostream>
#include <glm/glm.hpp>

#include "geometry.h"

class PolyBound {
 public:
  BoundingBox boundBox;
  int depth;
  std::list<RefFace> polySet;
  std::list<PolyBound> children;
  const Mesh* parentObject;

  ~PolyBound() = default;

  PolyBound(const Mesh* mesh, int d);

  void addPolygon(RefFace p);

  void split();

  bool mightIntersect(const glm::vec3& vec, const glm::vec3& origin, glm::vec3& res);

  IntersectionPoint intersect(const glm::vec3& vec, const glm::vec3& origin, SceneData& scene_data, RayTracerParams& scene_params);
};

#endif // !POLY_BOUND_H
