#include "ObjBound.h"

ObjBound::~ObjBound() {
  if (obj->type == SPHERE_OBJ) {
    delete boundBox;
  }
  else {
    delete myPolyBound;
  }
}

ObjBound::ObjBound(const GeoObject* o) : obj{ o } {
  if (obj->type() == SPHERE) {
    boundBox = new BoundingBox((const Sphere*)obj->data);
    myPolyBound = nullptr;
  }
  else {
    myPolyBound = new PolyBound((const Mesh*)o, 0);
    boundBox = &myPolyBound->boundBox;
  }
}

bool ObjBound::mightIntersect(const glm::vec3& vec, const glm::vec3& origin, glm::vec3& res) {
  return boundBox->intersect(vec, origin, res);
}

void ObjBound::split() {
  if (obj->type == POLYSET_OBJ) {
    myPolyBound->split();
  }
}

IntersectionPoint ObjBound::intersect(const glm::vec3& vec, const glm::vec3& origin, SceneData& scene_data, RayTracerParams& scene_params) {
  if (obj->type == SPHERE_OBJ) {
    return intersectSphere(vec, origin, obj, scene_data, scene_params);
  }
  else {
    return myPolyBound->intersect(vec, origin, scene_data, scene_params);
  }
}
