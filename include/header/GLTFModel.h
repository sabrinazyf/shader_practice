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
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    GLTFModel(string const &path, string const &binPath, bool gamma = false) {
        loadModel(path, binPath);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader_m &shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path, string const &binPath) {
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
//        bindModel(model);

        // 读取.bin文件（有可能也不需要在这读）
        std::ifstream f = ifstream(binPath, std::ios::binary);
        int membercount = 0;
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
            // 索引坐标（三角形的三个顶点的索引）
            vector<unsigned int> indices;
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
                for (const std::pair<const std::string, int> &attribute : primitive.attributes) {
                    const tinygltf::Accessor &accessor = model.accessors[attribute.second];
                    const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
                    // 这个attribute代表了点的位置POSITION
                    // 下面均copy and paste 来自 https://github.com/syoyo/tinygltf/issues/71

                    //下面分别获取position，normal和texcoord的数据
                    if ((attribute.first == "POSITION")) {
                        // bufferView byteoffset + accessor byteoffset tells you where the actual position data is within the buffer. From there
                        // you should already know how the data needs to be interpreted.
                        // 拿来存放从buffer获取到的数组的，buffer从.uri
                        const float *positions = reinterpret_cast<const float *>(&buffer.data[bufferView.byteOffset +
                                                                                              accessor.byteOffset]);
                        // From here, you choose what you wish to do with this position data. In this case, we  will display it out.
                        // 存储读取出的数据，并print出前10个数据
                        for (size_t i = 0; i < accessor.count; ++i) {
                            // Positions are Vec3 components, so for each vec3 stride, offset for x, y, and z.
                            if (i < 10) {
                                std::cout << "(" << positions[i * 3 + 0] << ", "// x
                                          << positions[i * 3 + 1] << ", " // y
                                          << positions[i * 3 + 2] << ")" // z
                                          << "\n";
                            }
                            glm::vec3 thisPos;
                            thisPos.x = positions[i * 3 + 0];
                            thisPos.y = positions[i * 3 + 1];
                            thisPos.z = positions[i * 3 + 2];
                            pos.push_back(thisPos);
                        }
                    }
                    if ((attribute.first == "NORMAL")) {
                        // bufferView byteoffset + accessor byteoffset tells you where the actual position data is within the buffer. From there
                        // you should already know how the data needs to be interpreted.
                        const float *normals = reinterpret_cast<const float *>(&buffer.data[bufferView.byteOffset +
                                                                                            accessor.byteOffset]);
                        for (size_t i = 0; i < accessor.count; ++i) {
                            if (i < 10) {
                                std::cout << "(" << normals[i * 3 + 0] << ", "// x
                                          << normals[i * 3 + 1] << ", " // y
                                          << normals[i * 3 + 2] << ")" // z
                                          << "\n";
                            }
                            glm::vec3 thisN;
                            thisN.x = normals[i * 3 + 0];
                            thisN.y = normals[i * 3 + 1];
                            thisN.z = normals[i * 3 + 2];
                            normal.push_back(thisN);
                        }
                    }
                    if ((attribute.first == "TEXCOORD_0")) {
                        // bufferView byteoffset + accessor byteoffset tells you where the actual position data is within the buffer. From there
                        // you should already know how the data needs to be interpreted.
                        const float *texc = reinterpret_cast<const float *>(&buffer.data[bufferView.byteOffset +
                                                                                         accessor.byteOffset]);
                        for (size_t i = 0; i < accessor.count; ++i) {
                            if (i < 10) {
                                std::cout << "(" << texc[i * 3 + 0] << ", "// x
                                          << texc[i * 3 + 1] << ", " // y
                                          << "\n";
                            }
                            glm::vec2 thisTexc;
                            thisTexc.x = texc[i * 3 + 0];
                            thisTexc.y = texc[i * 3 + 1];
                            texcoord.push_back(thisTexc);
                        }
                    }
                    // 在这可以帮忙检查一下三个vector有没有成功存储到数据了
                }
            }

        }
    }

    void bindModel(tinygltf::Model &model) {
        const tinygltf::Scene &scene = model.scenes[model.defaultScene];
        for (size_t i = 0; i < scene.nodes.size(); ++i) {
            assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
            bindModelNodes(model, model.nodes[scene.nodes[i]]);
        }
    }

    void bindModelNodes(tinygltf::Model &model, tinygltf::Node &node) {
        if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
            bindMesh(model, model.meshes[node.mesh]);
        }

        for (size_t i = 0; i < node.children.size(); i++) {
            assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
            bindModelNodes(model, model.nodes[node.children[i]]);
        }
    }

    void bindMesh(tinygltf::Model &model, tinygltf::Mesh &mesh) {
        for (size_t i = 0; i < model.bufferViews.size(); ++i) {
            const tinygltf::BufferView &bufferView = model.bufferViews[i];
            if (bufferView.target == 0) {  // TODO impl drawarrays
                std::cout << "WARN: bufferView.target is zero" << std::endl;
                continue;  // Unsupported bufferView.
            }

            const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

//            glBufferData(bufferView.target, bufferView.byteLength,
//                         &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

            cout << &buffer.data.at(0) + bufferView.byteOffset << " ";
            cout << bufferView.byteLength << " " << endl;
        }

        cout << endl;

        for (size_t i = 0; i < mesh.primitives.size(); ++i) {
            tinygltf::Primitive primitive = mesh.primitives[i];
            tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

            for (auto &attrib : primitive.attributes) {
                tinygltf::Accessor accessor = model.accessors[attrib.second];
                int byteStride =
                        accessor.ByteStride(model.bufferViews[accessor.bufferView]);

                int size = 1;
                if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                    size = accessor.type;
                }
            }

            if (model.textures.size() > 0) {
                // fixme: Use material's baseColor
                tinygltf::Texture &tex = model.textures[0];

                if (tex.source > -1) {
                    tinygltf::Image &image = model.images[tex.source];
                }
            }
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
