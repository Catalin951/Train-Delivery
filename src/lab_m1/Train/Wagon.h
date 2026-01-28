#pragma once

#include "lab_m1/Train/rail.h"
#include "components/simple_scene.h"
#include "utils/glm_utils.h"
#include "lab_m1/Train/renderable.h"

class Wagon : public Renderable {
public:
    Wagon(glm::vec3 position);
    ~Wagon();
    void Render(
        const std::unordered_map<std::string, Mesh*>& meshes,
        const std::unordered_map<std::string, Shader*>& shaders,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix) override;

    void SetPosition(glm::vec3 position);

    void SetRotation(float angle);

    // Base Plate (Chassis)
    const float BASE_LENGTH = 4.0f;    // Length (X scale factor)
    const float BASE_WIDTH = 1.5f;     // Width  (Z scale factor)
    const float BASE_HEIGHT = 0.2f;    // Height (Y scale factor)
    const float BASE_Y_OFFSET = 0.5f;  // Y-translation to lift it off the ground
    const float BASE_X_OFFSET = -2.f;  // X-translation offset
    const float BASE_Z_OFFSET = -0.75f;  // Z-translation offset


    // Cabin
    const float CABIN_LENGTH = 4.0f;     // Length (X scale factor)
    const float CABIN_HEIGHT = 1.75f;    // Height (Y scale factor)
    const float CABIN_WIDTH = 1.5f;      // Width  (Z scale factor)
    const float CABIN_X_OFFSET = -2.0f;  // X-Translation position
    const float CABIN_Y_OFFSET = 0.5f + 0.2f;  // Y-Translation position
    const float CABIN_Z_OFFSET = -0.75f;  // Z-Translation position

    // Wheels (7 on each side)
    const int NUM_WHEELS_SIDE = 2;
    const float WHEEL_SCALE_R = 0.25f;  // Radius Scale Factor
    const float WHEEL_SCALE_W = 0.1f;   // Width Scale Factor
    const float WHEEL_X_START = -1.5f;  // Starting X offset for the first wheel
    const float WHEEL_X_SPACING = 3; // Spacing between wheels
    const float L_WHEEL_Z_OFFSET = 0.5f - 0.1f;  // Z-position (distance from center line)
    const float R_WHEEL_Z_OFFSET = -0.5f;  // Z-position (distance from center line)

    const float WHEEL_Y_OFFSET = 0.25f;

private:
    float angle;
    float progress;  // 0.f to 1.f
    float speed;
    glm::vec3 position;
};