#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <algorithm>
#include <glm/glm.hpp>
#include "IntersectionPrimitives.h"

class BoundingBox {
 public:
  glm::vec3 vMax;
  glm::vec3 vMin;

  BoundingBox();

  BoundingBox(RefFace face);

  // BoundingBox(const PolygonIO* poly);

  // BoundingBox(const SphereIO* sphere);

  void apply(const BoundingBox& box);

  bool intersect(const glm::vec3& vec, const glm::vec3& origin, glm::vec3& res);

  bool inside(glm::vec3 point);
};

// BoundingBox boundScene(SceneIO* scene);

BoundingBox BoundMesh(const Mesh& mesh);

#endif // !BOUNDING_BOX_H
