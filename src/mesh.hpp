#ifndef MESH_HPP
#define MESH_HPP

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>
#include <vector>

#include <assimp/types.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// GL Includes
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "material.h"
#include "shader.hpp"
#include "geometry.h"

namespace shared_obj {

  struct OpenglVars;

  class Mesh : public GeoObject {
  public:    
    /*  Mesh Data  */
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    VertexMaterialType material_type;
    GLuint VAO;

    /*  Functions  */
    // Constructor
    Mesh(std::vector<Vertex> vertices,
	 std::vector<GLuint> indices,
	 std::vector<Texture> textures,
	 VertexMaterialType material_type);

    GeoObjectType obj_type() const override {
      return MESH;
    }

    // Render the mesh
    void Draw(Shader shader, bool use_textures);

  private:
    /*  Render data  */
    GLuint VBO, EBO;

    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh();
  };

}  // namespace shared_obj

#endif
