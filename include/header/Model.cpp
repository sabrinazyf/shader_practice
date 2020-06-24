//
// Created by 霏霏 on 2020/6/24.
//

#include "Model.h"

Model::Model(const string &path, bool gamma) : gammaCorrection(gamma)
{
    loadModel(path);
}

void Model::Draw(Shader &shader) {
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}
