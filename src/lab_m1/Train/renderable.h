#pragma once

#include "core/gpu/mesh.h"
#include "core/gpu/shader.h"
#include "utils/glm_utils.h"
#include <unordered_map>

class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void Render(
        const std::unordered_map<std::string, Mesh*>& meshes,
        const std::unordered_map<std::string, Shader*>& shaders,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix) = 0;
protected:
    void RenderMeshCustom(
        Mesh* mesh,
        Shader* shader,
        const glm::mat4& modelMatrix,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix);
};