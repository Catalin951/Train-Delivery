#pragma once

#include "utils/glm_utils.h"
#include "core/gpu/mesh.h"

namespace object3D
{
    Mesh* CreateParallelepiped(
        const std::string& name,
        glm::vec3 leftBottomBackCorner,
        float length,
        float height,
        float depth,
        glm::vec3 color);

    Mesh* CreateCylinder(
        const std::string& name,
        glm::vec3 centerBase,
        float radius,
        float length, // length (on Y axis)
        glm::vec3 color);

    Mesh* CreateSphere(const std::string& name, glm::vec3 center, float radius, glm::vec3 color);

}   // namespace object3D