#include <vector>

#include <assimp/types.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>

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

struct RefFace {
  const Vertex& verts[3];

  RefFace(const Vertex& a,
	  const Vertex& b,
	  const Vertex& c){
    verts[0] = a;
    verts[1] = b;
    verts[2] = c;
  }

  RefFace() = default;
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

enum GeoObjectType {
  SPHERE = 1,
  MESH = 2,
};

// Temporary abstract class
class GeoObject {
 public:
  virtual GeoObjectType obj_type() const = 0;
};
