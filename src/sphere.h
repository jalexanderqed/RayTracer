//
// Created by jalexander on 10/13/18.
//

#ifndef GLITTER_SPHERE_H
#define GLITTER_SPHERE_H

#include <glm/glm.hpp>

#include material.h
#include geometry.h

namespace shared_obj {

  class Sphere : public GeoObject {
  public:
  Sphere(glm::vec3 o, float r,
	 glm::vec3 x_a, float x_l,
	 glm::vec3 y_a, float y_l,
	 glm::vec3 z_a, float z_l,
	 Material mat)
    : origin(o), radius(r),
      x_axis(x_a), x_length(x_l),
      y_axis(y_a), y_length(y_l),
      z_axis(z_a), z_length(z_l),
      material(mat) {}

    GeoObjectType obj_type() const override {
      return SPHERE;
    }
    
    Material material;
    glm::vec3 origin;   /* Origin of the sphere/ellipsoid		*/
    float radius;
    glm::vec3 x_axis;	    /* Direction of the x axis			*/
    float x_length;    /* Length of the x axis			*/
    glm::vec3 y_axis;	    /* Direction of the y axis			*/
    float y_length;    /* Length of the y axis			*/
    glm::vec3 z_axis;	    /* Direction of the z axis			*/
    float z_length;    /* Length of the z axis			*/
  };

}  // namespace shared_obj

#endif //GLITTER_SPHERE_H
