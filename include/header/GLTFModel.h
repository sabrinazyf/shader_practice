#ifndef SHADERPRACTICE_GLTFMODEL_H
#define SHADERPRACTICE_GLTFMODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
#include "Shader_m.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include <tiny_gltf.h>

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class GLTFModel {
public:
    // model data
    vector<Texture> textures_loaded;    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection{};

    // constructor, expects a filepath to a 3D model.
    explicit GLTFModel(string const &path, bool gamma = false) {
        directory = path;
        gammaCorrection = gamma;
        loadModel(path);
    }

    GLTFModel(Mesh &mesh, bool gamma = false) {
        meshes.push_back(mesh);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader_m &shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
//        meshes[0].Draw(shader);
    }

    void Draw(vector<Shader_m> &shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader.at(i));
//        meshes[0].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path) {
//        fx::gltf::Document helmet = fx::gltf::LoadFromText(path);
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

        if (!warn.empty()) {
            printf("Warn: %s\n", warn.c_str());
        }

        if (!err.empty()) {
            printf("Err: %s\n", err.c_str());
        }

        if (!ret) {
            printf("Failed to parse glTF\n");
        }

        // 对于包含在model里的每一个mesh
        for (int i = 0; i < model.meshes.size(); ++i) {
            tinygltf::Mesh mesh = model.meshes[i];
            // 用于构造header/Mesh类的东西
            vector<Vertex> vertices;
            // 点的位置
            vector<glm::vec3> pos;
            // 点的法线向量
            vector<glm::vec3> normal;
            // 贴图坐标
            vector<glm::vec2> texcoord;
            // 偏移坐标
            vector<glm::vec3> tangent;
            // 索引坐标（三角形的三个顶点的索引）
            vector<GLuint> indices;
            //贴图
            vector<Texture> textures;

            // 给这加个断点
            // model.accessors可以看到bin数据的几个组成部分，
            // model.accessors[i].count代表了当前组成部分所包含的元素数量
            // *在Dragon模型中，总共有100000个三角形，accessors对应索引坐标的count为300000
            // *在Dragon模型中，顶点的个数为6万多个，对应的点的位置、法线、贴图坐标也是6万多个
            // *这个dragon模型的gltf分为两部分，第一部分是龙，第二部分是一个平面
            // accessors的0-4项属于龙，5-9项属于平面

//            mesh的结构：
//            {
//                "name" : "default",
//                        "primitives" : [
//                {
//                    "attributes" : {
//                                "NORMAL" : 2,
//                                "POSITION" : 1,
//                                "TANGENT" : 3,
//                                "TEXCOORD_0" : 4
//                    },
//                    "indices" : 0,  目前我需要获取到这个对应的array buffer并转换为int型数组，但是找不到教程，
//                                    强制按位置读取出来的是short，直接强制类型转换会出现负数值
//                            "material" : 1
//                }
//                ]
//            },
            // 简单的数据解读和强制按位置读取的教程：https://blog.csdn.net/qq_31709249/article/details/86535797
            for (const tinygltf::Primitive &primitive : mesh.primitives) {

                const tinygltf::Accessor &accessor = model.accessors[primitive.indices];
                const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
                const auto *thisIndices = reinterpret_cast<const short *>(&buffer.data[bufferView.byteOffset +
                                                                                        accessor.byteOffset]);
                for (size_t ii = 0; ii < accessor.count; ++ii) {
                    int a = thisIndices[ii] & 0xffff;
                    indices.push_back(a);
                }

                for (const std::pair<const std::string, int> &attribute : primitive.attributes) {
                    const tinygltf::Accessor &accessor_p = model.accessors[attribute.second];
                    const tinygltf::BufferView &bufferView_p = model.bufferViews[accessor_p.bufferView];
                    const tinygltf::Buffer &buffer_p = model.buffers[bufferView_p.buffer];
                    // 这个attribute代表了点的位置POSITION
                    // 下面均copy and paste 来自 https://github.com/syoyo/tinygltf/issues/71

                    //下面分别获取position，normal和texcoord的数据
                    if ((attribute.first == "POSITION")) {
                        // bufferView byteoffset + accessor byteoffset tells you where the actual position data is within the buffer. From there
                        // you should already know how the data needs to be interpreted.
                        // 拿来存放从buffer获取到的数组的，buffer从.uri
                        const auto *positions = reinterpret_cast<const float *>(&buffer_p.data[bufferView_p.byteOffset +
                                                                                               accessor_p.byteOffset]);
                        // From here, you choose what you wish to do with this position data. In this case, we  will display it out.
                        // 存储读取出的数据，并print出前10个数据
//                        cout << accessor_p.count << endl;
                        for (size_t ii = 0; ii < accessor_p.count; ++ii) {
                            // Positions are Vec3 components, so for each vec3 stride, offset for x, y, and z.
//                            if (i < 10) {
//                                std::cout << "(" << positions[i * 3 + 0] << ", "// x
//                                          << positions[i * 3 + 1] << ", " // y
//                                          << positions[i * 3 + 2] << ")" // z
//                                          << "\n";
//                            }
                            glm::vec3 thisPos;
                            thisPos.x = positions[ii * 3 + 0];
                            thisPos.y = positions[ii * 3 + 1];
                            thisPos.z = positions[ii * 3 + 2];
                            pos.push_back(thisPos);
                        }
                    }
                    if ((attribute.first == "NORMAL")) {
                        // bufferView byteoffset + accessor byteoffset tells you where the actual position data is within the buffer. From there
                        // you should already know how the data needs to be interpreted.
                        const auto *normals = reinterpret_cast<const float *>(&buffer_p.data[bufferView_p.byteOffset +
                                                                                             accessor_p.byteOffset]);
                        for (size_t ii = 0; ii < accessor_p.count; ++ii) {
//                            if (i < 10) {
//                                std::cout << "(" << normals[i * 3 + 0] << ", "// x
//                                          << normals[i * 3 + 1] << ", " // y
//                                          << normals[i * 3 + 2] << ")" // z
//                                          << "\n";
//                            }
                            glm::vec3 thisN;
                            thisN.x = normals[ii * 3 + 0];
                            thisN.y = normals[ii * 3 + 1];
                            thisN.z = normals[ii * 3 + 2];
                            normal.push_back(thisN);
                        }
                    }

                    if ((attribute.first == "TANGENT")) {
                        // bufferView byteoffset + accessor byteoffset tells you where the actual position data is within the buffer. From there
                        // you should already know how the data needs to be interpreted.
                        const auto *tan = reinterpret_cast<const float *>(&buffer_p.data[bufferView_p.byteOffset +
                                                                                         accessor_p.byteOffset]);
                        for (size_t ii = 0; ii < accessor_p.count; ++ii) {
//                            if (i < 10) {
//                                std::cout << "(" << normals[i * 3 + 0] << ", "// x
//                                          << normals[i * 3 + 1] << ", " // y
//                                          << normals[i * 3 + 2] << ")" // z
//                                          << "\n";
//                            }
                            glm::vec3 thisT;
                            thisT.x = tan[ii * 4 + 0];
                            thisT.y = tan[ii * 4 + 1];
                            thisT.z = tan[ii * 4 + 2];
                            tangent.push_back(thisT);
                        }
                    }

                    if ((attribute.first == "TEXCOORD_0")) {
                        // bufferView byteoffset + accessor byteoffset tells you where the actual position data is within the buffer. From there
                        // you should already know how the data needs to be interpreted.
                        const auto *texc = reinterpret_cast<const float *>(&buffer_p.data[bufferView_p.byteOffset +
                                                                                          accessor_p.byteOffset]);
                        for (size_t ii = 0; ii < accessor_p.count; ++ii) {
//                            if (i < 10) {
//                                std::cout << "(" << texc[i * 3 + 0] << ", "// x
//                                          << texc[i * 3 + 1] << ")" // y
//                                          << "\n";
//                            }
                            glm::vec2 thisTexc;
                            thisTexc.x = texc[ii * 2 + 0];
                            thisTexc.y = texc[ii * 2 + 1];
                            texcoord.push_back(thisTexc);
                        }
                    }
                    // 在这可以帮忙检查一下三个vector有没有成功存储到数据了
                }
            }
            for (int ii = 0; ii < pos.size(); ii++) {
                Vertex vertex{};
                vertex.Position = pos.at(ii);
                vertex.Normal = normal.at(ii);
                vertex.TexCoords = texcoord.at(ii);
                vertices.push_back(vertex);
            }
//            cout << "vertex count: "<<vertices.size() << endl;
//            cout << "mesh count: "<<indices.size() << endl;
            Mesh thisMesh(vertices, indices, textures);
            meshes.emplace_back(thisMesh);
//            cout << "Push one mesh" << endl;
        }
    }
};


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma) {
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

#endif //SHADERPRACTICE_GLTFMODEL_H
