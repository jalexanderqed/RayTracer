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

namespace shared_obj {

  struct OpenglVars;

  enum VertexMaterialType{
    TEXTURE,
    MATERIAL,
  };

  struct Vertex {
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
    // Tangent
    glm::vec3 Tangent;
    // Bitangent
    glm::vec3 Bitangent;

    Material material;
  };

  struct Texture {
    GLuint id;
    std::string type;
    aiString path;
    unsigned char* image;
    int image_width;
    int image_height;
    int image_channels;

    ~Texture(){
      if(image != nullptr) {
	stbi_image_free(image);
      }
    }
  };

  class Mesh {
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
