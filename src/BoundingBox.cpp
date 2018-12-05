#include "BoundingBox.h"

BoundingBox::BoundingBox() : vMin{ glm::vec3(INFINITY) },
			     vMax{ glm::vec3(-1 * INFINITY) } { }

BoundingBox::BoundingBox(RefFace face) {
  for (int i = 0; i < 3; i++) {
    vMax[i] = max(face.verts[0].position[i],
		  max(face.verts[1].Position[i],
		      face.verts[2].Position[i]));
    vMin[i] = min(face.verts[0].position[i],
		  min(face.verts[1].Position[i],
		      face.verts[2].Position[i]));
  }
}

/*
BoundingBox::BoundingBox(const SphereIO* sphere) {
  for (int i = 0; i < 3; i++) {
    vMax[i] = sphere->origin[i] + sphere->radius;
    vMin[i] = sphere->origin[i] - sphere->radius;
  }
}
*/

void BoundingBox::apply(const BoundingBox& box) {
  for (int i = 0; i < 3; i++) {
    vMax[i] = max(vMax[i], box.vMax[i]);
    vMin[i] = min(vMin[i], box.vMin[i]);
  }
}

bool BoundingBox::intersect(const glm::vec3& vec, const glm::vec3& origin, glm::vec3& res) {
  glm::vec3 closePlanes(0);
  glm::vec3 farPlanes(0);
  glm::vec3 invDir(1.0f / vec.x, 1.0f / vec.y, 1.0f / vec.z);
  for (int i = 0; i < 3; i++) {
    if (invDir[i] < 0) {
      closePlanes[i] = vMax[i];
      farPlanes[i] = vMin[i];
    }
    else {
      closePlanes[i] = vMin[i];
      farPlanes[i] = vMax[i];
    }
  }

  float tMin = (closePlanes.x - origin.x) * invDir.x;
  float tMax = (farPlanes.x - origin.x) * invDir.x;
  float tyMin = (closePlanes.y - origin.y) * invDir.y;
  float tyMax = (farPlanes.y - origin.y) * invDir.y;

  if (tMin > tyMax || tyMin > tMax) return false;
  if (tyMin > tMin) tMin = tyMin;
  if (tyMax < tMax) tMax = tyMax;

  float tzMin = (closePlanes.z - origin.z) * invDir.z;
  float tzMax = (farPlanes.z - origin.z) * invDir.z;

  if (tMin > tzMax || tzMin > tMax) return false;
  if (tzMin > tMin) tMin = tzMin;
  if (tzMax < tMax) tMax = tzMax;

  if (tMin <= 0) {
    if (tMax <= 0) return false;
    else res = origin + tMax * vec;
  }
  else {
    res = origin + tMin * vec;
  }

  return true;
}

bool BoundingBox::inside(glm::vec3 point) {
  return point.x < vMax.x &&
		   point.x > vMin.x &&
    point.y < vMax.y &&
	      point.y > vMin.y &&
    point.z < vMax.z &&
	      point.z > vMin.z;
}

BoundingBox BoundMesh(const Mesh& mesh) {
  BoundingBox res;
  for (int i = 0; i < mesh.vertices.size(); i += 3) {
    res.apply(BoundingBox(RefFace(mesh.vertices[i],
				  mesh.vertices[i + 1],
				  mesh.vertices[i + 2])));
  }
  return res;
}

/*
BoundingBox boundScene(SceneIO* scene) {
  BoundingBox res;
  for (ObjIO *object = scene->objects; object != NULL; object = object->next) {
    IntersectionPoint currPoint;
    if (object->type == SPHERE_OBJ) {
      res.apply(BoundingBox((SphereIO *)(object->data)));
    }
    else {
      res.apply(boundPolySet((PolySetIO*)(object->data)));
    }
  }
  return res;
}
*/
