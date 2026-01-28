#include "lab_m1/Train/object3D.h"

#include <vector>
#include <cmath>

#include "core/engine.h"
#include "utils/gl_utils.h"
#include "core/gpu/mesh.h"

Mesh* object3D::CreateParallelepiped(
    const std::string& name,
    glm::vec3 leftBottomBackCorner,
    float length, // Length (X)
    float height, // Height (Y)
    float depth,  // Depth (Z)
    glm::vec3 color)
{
    glm::vec3 corner = leftBottomBackCorner;

    std::vector<VertexFormat> vertices =
    {
        // Front Face (Z = depth)
        VertexFormat(corner + glm::vec3(0, 0, depth), color),          // 0: Left-Bottom-Front
        VertexFormat(corner + glm::vec3(length, 0, depth), color),     // 1: Right-Bottom-Front
        VertexFormat(corner + glm::vec3(length, height, depth), color),// 2: Right-Top-Front
        VertexFormat(corner + glm::vec3(0, height, depth), color),     // 3: Left-Top-Front

        // Back Face (Z = 0)
        VertexFormat(corner + glm::vec3(0, 0, 0), color),              // 4: Left-Bottom-Back
        VertexFormat(corner + glm::vec3(length, 0, 0), color),         // 5: Right-Bottom-Back
        VertexFormat(corner + glm::vec3(length, height, 0), color),    // 6: Right-Top-Back
        VertexFormat(corner + glm::vec3(0, height, 0), color)          // 7: Left-Top-Back
    };

    Mesh* cube = new Mesh(name);
    std::vector<unsigned int> indices =
    {
        // Front Face
        0, 1, 2,    0, 2, 3,
        // Back Face
        5, 4, 7,    5, 7, 6,
        // Left Face
        4, 0, 3,    4, 3, 7,
        // Right Face
        1, 5, 6,    1, 6, 2,
        // Top Face
        3, 2, 6,    3, 6, 7,
        // Bottom Face
        4, 5, 1,    4, 1, 0
    };

    cube->InitFromData(vertices, indices);
    return cube;
}

Mesh* object3D::CreateCylinder(
    const std::string& name,
    glm::vec3 centerBase,
    float radius,
    float length, // Cylinder length (along Y-axis)
    glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Cylinder number of slices
    int segments = 30;

    // Base centers
    vertices.push_back(VertexFormat(centerBase, color)); // 0: Bottom base center
    vertices.push_back(VertexFormat(centerBase + glm::vec3(0, length, 0), color)); // 1: Top base center

    // Generate points on the circle circumference
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle); // Using XZ for the circle, Y for height

        // Bottom base point
        vertices.push_back(VertexFormat(centerBase + glm::vec3(x, 0, z), color));
        // Top base point
        vertices.push_back(VertexFormat(centerBase + glm::vec3(x, length, z), color));
    }

    // Indices
    for (int i = 0; i < segments; i++) {
        // Offset to find the correct indices in the vector
        int bottom1 = 2 + 2 * i;
        int top1 = 2 + 2 * i + 1;
        int bottom2 = 2 + 2 * ((i + 1) % segments);
        int top2 = 2 + 2 * ((i + 1) % segments) + 1;

        // Bottom base triangles
        indices.push_back(0);
        indices.push_back(bottom2);
        indices.push_back(bottom1);

        // Top base triangles
        indices.push_back(1);
        indices.push_back(top1);
        indices.push_back(top2);

        // Side faces (2 triangles per segment)
        indices.push_back(bottom1);
        indices.push_back(bottom2);
        indices.push_back(top1);

        indices.push_back(bottom2);
        indices.push_back(top2);
        indices.push_back(top1);
    }

    Mesh* cylinder = new Mesh(name);
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}

Mesh* object3D::CreateSphere(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color)
{
    int stacks = 20; // Lat
    int sectors = 20; // Long

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;
        float y = radius * cos(phi);
        float r = radius * sin(phi);

        for (int j = 0; j <= sectors; ++j) {
            float theta = 2.0f * M_PI * j / sectors;
            float x = r * cos(theta);
            float z = r * sin(theta);

            vertices.push_back(VertexFormat(center + glm::vec3(x, y, z), color));
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);     // beginning of current stack
        int k2 = k1 + sectors + 1;      // beggining of next stack

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            // 2 triangles per square
            // k1    k1+1
            //         
            // k2    k2+1

            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    Mesh* sphere = new Mesh(name);
    sphere->InitFromData(vertices, indices);
    return sphere;
}