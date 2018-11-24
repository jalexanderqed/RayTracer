//
// Created by jalexander on 10/13/18.
//

#ifndef GLITTER_SPHERE_H
#define GLITTER_SPHERE_H

#include <glm/glm.hpp>

#include material.h

namespace shared_obj {

  class Sphere {
  public:
  Sphere(glm::vec3 origin, float radius,
	 glm::vec3 x_axis, float x_length,
	 glm::vec3 y_axis, float y_length,
	 glm::vec3 z_axis, float z_length,
	 Material material)
    : origin_(origin), radius_(radius),
      x_axis_(x_axis), x_length_(x_length),
      y_axis_(y_axis), y_length_(y_length),
      z_axis_(z_axis), z_length_(z_length),
      material_(material) {}
    
  private:
    Material material_;
    glm::vec3 origin_;   /* Origin of the sphere/ellipsoid		*/
    float radius_;
    glm::vec3 x_axis_;	    /* Direction of the x axis			*/
    float x_length_;    /* Length of the x axis			*/
    glm::vec3 y_axis_;	    /* Direction of the y axis			*/
    float y_length_;    /* Length of the y axis			*/
    glm::vec3 z_axis_;	    /* Direction of the z axis			*/
    float z_length_;    /* Length of the z axis			*/
  };

}  // namespace shared_obj

#endif //GLITTER_SPHERE_H
