#ifndef MESH_HPP
#define MESH_HPP

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <assimp/types.h>

// GL Includes
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"

namespace gl_code {

    struct OpenglVars;

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
    };

    struct Texture {
        GLuint id;
        std::string type;
        aiString path;
    };

    class Mesh {
    public:
        /*  Mesh Data  */
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;
        GLuint VAO;

        /*  Functions  */
        // Constructor
        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

        // Render the mesh
        void Draw(Shader shader, bool use_textures);

    private:
        /*  Render data  */
        GLuint VBO, EBO;

        /*  Functions    */
        // Initializes all the buffer objects/arrays
        void setupMesh();
    };

}  // namespace gl_code

#endif
