#include "ShaderStore.h"

ShaderStore::ShaderStore() {
    standardModelShader = LoadShader("../shaders/vs.glsl", "../shaders/fs.glsl");
    int ambientLoc = GetShaderLocation(standardModelShader, "ambientColor");
    float ambColor[3] = { 1.0f, 1.0f, 1.0f };
    SetShaderValue(standardModelShader, ambientLoc, ambColor, SHADER_UNIFORM_VEC3);
}

ShaderStore::~ShaderStore() {
  UnloadShader(standardModelShader);
}
