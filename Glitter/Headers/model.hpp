#ifndef MODEL_HPP
#define MODEL_HPP

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;
// GL Includes
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

#include "mesh.hpp"

namespace shared_obj {

    struct OpenglVars;

    GLint TextureFromFile(const char *path, string directory, bool gamma = false);

    class Model {
    public:
        /*  Model Data */
        vector<Texture> textures_loaded;    // Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        vector<Mesh> meshes;
        string directory;
        bool gammaCorrection;

        /*  Functions   */
        // Constructor, expects a filepath to a 3D model.
        Model(string const &path, bool gamma = false);

        // Draws the model, and thus all its meshes
        void Draw(Shader shader, bool use_textures);

    private:
        /*  Functions   */
        // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(string path);

        // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode *node, const aiScene *scene);

        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        // Checks all material textures of a given type and loads the textures if they're not loaded yet.
        // The required info is returned as a Texture struct.
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    };


    Texture TextureFromFile(const char *path, string directory, bool gamma);

}  // namespace shared_obj

#endif