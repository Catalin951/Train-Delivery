#pragma once

#include "lab_m1/Train/rail.h"
#include "components/simple_scene.h"
#include "utils/glm_utils.h"
#include "lab_m1/Train/Wagon.h"
#include "lab_m1/Train/renderable.h"

enum ResourceType {
    NONE = 0,
    WOOD,
    OIL,
    STONE
};

class Train : public Renderable {
public:
    float angle = 0.f;
    void SetRail(Rail* rail);
    void SetSpeed(float s);
    void ResolveIntersection(int directionCode);
    float GetAngleDiff(glm::vec3 dirA, glm::vec3 dirB);
    Train(glm::vec3 position, float angleDeg);
    ~Train();
    void Render(
        const std::unordered_map<std::string, Mesh*>& meshes,
        const std::unordered_map<std::string, Shader*>& shaders,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix) override;
    void Update(float deltaTimeSeconds);

    // CONSTANTS FOR DIRECTIONS
    static const int DIR_FORWARD = 0;
    static const int DIR_SOFT_LEFT = -1;
    static const int DIR_SOFT_RIGHT = 1;
    static const int DIR_HARD_LEFT = -2;
    static const int DIR_HARD_RIGHT = 2;
    static const int DIR_SHARP_LEFT = -3;
    static const int DIR_SHARP_RIGHT = 3;

    // GLOBAL LOCOMOTIVE DIMENSIONS & OFFSETS

    // Base Plate (Chassis)
    const float BASE_LENGTH = 4.0f;    // Length (X scale factor)
    const float BASE_WIDTH = 1.5f;     // Width  (Z scale factor)
    const float BASE_HEIGHT = 0.2f;    // Height (Y scale factor)
    const float BASE_Y_OFFSET = 0.5f;  // Y-translation to lift it off the ground
    const float BASE_X_OFFSET = -2.f;  // X-translation offset
    const float BASE_Z_OFFSET = -0.75f;  // Z-translation offset


    // Cabin
    const float CABIN_LENGTH = 1.5f;     // Length (X scale factor)
    const float CABIN_HEIGHT = 1.75f;    // Height (Y scale factor)
    const float CABIN_WIDTH = 1.5f;      // Width  (Z scale factor)
    const float CABIN_X_OFFSET = -2.0f;  // X-Translation position
    const float CABIN_Y_OFFSET = 0.5f + 0.2f;  // Y-Translation position
    const float CABIN_Z_OFFSET = -0.75f;  // Z-Translation position


    // Engine (Outer Cylinder)
    const float ENGINE_X_OFFSET = 1.5f;  // X-Translation position
    const float ENGINE_Y_OFFSET = 1.2f;  // Y-Translation position
    const float ENGINE_Z_OFFSET = 0;     // Z-Translation position
    const float ENGINE_SCALE_R  = 1.0f;  // Radius Scale Factor (X/Z)
    const float ENGINE_SCALE_L  = 3.0f;  // Length Scale Factor (Y/Z)

    // Inner Cylinder
    const float INNER_ENGINE_X_OFFSET = 1.6f; // X-Translation position
    const float INNER_ENGINE_Y_OFFSET = 1.2f; // Y-Translation position
    const float INNER_SCALE_R = 0.25f;   // Radius Scale Factor (X/Z)
    const float INNER_SCALE_L = 2.8f;   // Length Scale Factor (Y/Z, after rotation)

    // Wheels (7 on each side)
    const int NUM_WHEELS_SIDE = 7;
    const float WHEEL_SCALE_R = 0.25f;  // Radius Scale Factor
    const float WHEEL_SCALE_W = 0.1f;   // Width Scale Factor
    const float WHEEL_X_START = -1.5f;  // Starting X offset for the first wheel
    const float WHEEL_X_SPACING = 0.5f; // Spacing between wheels
    const float L_WHEEL_Z_OFFSET = 0.5f - 0.1f;  // Z-position (distance from center line)
    const float R_WHEEL_Z_OFFSET = -0.5f;  // Z-position (distance from center line)

    const float WHEEL_Y_OFFSET = 0.25f;
    const float WAGON_OFFSET_X = -5.f;
    bool isWaitingForPath = false;
    Rail* currentRail;
    float distanceTraveled; // how much we traveled on the rail
    std::vector<Wagon *> wagons;
    int totalWagons = 1;
    float progress;  // 0.f to 1.f
    float speed = 0;
    glm::vec3 position;
};