#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader_m.h"

#include <string>
#include <vector>

using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    // render the mesh
    void Draw(Shader_m &shader) const;

//    float* getVerticesArray() {
//        float verticesArray[vertices.size() * 3];
//        for(int i=0;i<vertices.size();i++){
//            verticesArray[i*3] = vertices.
//        }
//    }

private:
    // render data
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};

#endif