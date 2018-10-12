//
// Created by jalexander on 10/6/18.
//

#include "model.hpp"

namespace shared_obj {

    Model::Model(string
                 const &path,
                 bool gamma
    ) :
            gammaCorrection(gamma) {
        this->loadModel(path);
    }

    void Model::Draw(Shader shader, bool use_textures) {
        for (GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Draw(shader, use_textures);
    }

    void Model::loadModel(string
                          path) {
        // Read file via ASSIMP
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                                       aiProcess_CalcTangentSpace);
        // Check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // Retrieve the directory path of the filepath
        this->directory = path.substr(0, path.find_last_of('/'));

        // Process ASSIMP's root node recursively
        this->processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode *node, const aiScene *scene) {
        // Process each mesh located at the current node
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            // The node object only contains indices to index the actual objects in the scene.
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (GLuint i = 0; i < node->mNumChildren; i++) {
            this->processNode(node->mChildren[i], scene);
        }

    }

    Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
        // Data to fill
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;

        // Walk through each of the mesh's vertices
        for (GLuint i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // Texture Coordinates
            if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            if (mesh->mTangents) {
                // Tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
            }
            if (mesh->mBitangents) {
                // Bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            vertices.push_back(vertex);
        }
        // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (GLuint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them in the indices vector
            for (GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // Process materials
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // Diffuse: texture_diffuseN
        // Specular: texture_specularN
        // Normal: texture_normalN

        // 1. Diffuse maps
        vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE,
                                                                 "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. Specular maps
        vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR,
                                                                  "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. Normal maps
        std::vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT,
                                                                     "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. Height maps
        std::vector<Texture> heightMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT,
                                                                     "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
        vector<Texture> textures;
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;
            for (GLuint j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip) {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                this->textures_loaded.push_back(
                        texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }


    Texture TextureFromFile(const char *path, string directory, bool gamma) {
        //Generate texture ID and load texture data
        string filename = string(path);
        filename = directory + '/' + filename;
        Texture new_texture;
        while(filename.find('\\') != string::npos){
            int pos = filename.find('\\');
            filename.replace(pos, 1, "/");
        }
        glGenTextures(1, &new_texture.id);
        new_texture.image = stbi_load(filename.c_str(),
                &new_texture.image_width,
                &new_texture.image_height,
                &new_texture.image_channels,
                gamma ? 4 : 3);
        if(new_texture.image == NULL){
            std::cerr << stbi_failure_reason() << " " << filename << std::endl;
            new_texture.id = -1;
            return new_texture;
        }
        // Assign texture to ID
        glBindTexture(GL_TEXTURE_2D, new_texture.id);
        glTexImage2D(GL_TEXTURE_2D, 0, gamma ? GL_SRGB : GL_RGB,
                new_texture.image_width,
                new_texture.image_height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                new_texture.image);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        return new_texture;
    }

}  // namespace shared_obj