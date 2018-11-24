#ifndef MATERIAL_H
#define MATERIAL_H

namespace shared_obj {

  struct Material{
    glm::vec3 diffuse_color;

    glm::vec3 ambient_color;

    glm::vec3 specular_color;

    glm::vec3 emissive_color;

    // 0.0 - 1.0
    // Must be scaled (multiply by 128) before use as a Phong
    float shininess;
    
    // Transparency, 0.0-1.0
    float ktran;
  };

}  // shared_obj

#endif  // MATERIAL_H
