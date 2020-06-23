//
// Created by 霏霏 on 2020/6/24.
//

#include "Shader.h"

void Shader::Use() const {
    glUseProgram(this->Program);
}
