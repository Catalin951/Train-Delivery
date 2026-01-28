#include "lab_m1/Train/Train.h"
#include "lab_m1/Train/rail.h"
#include "utils/glm_utils.h"
#include "lab_m1/Train/transform3D.h"
#include "lab_m1/Train/Wagon.h"
#include "lab_m1/Train/renderable.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

void Wagon::Render(const std::unordered_map<std::string, Mesh*>& meshes,
    const std::unordered_map<std::string, Shader*>& shaders,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix)
{
    glm::mat4 wagonBodyMatrix = glm::mat4(1);
    wagonBodyMatrix *= transform3D::Translate(position.x, position.y, position.z);
    wagonBodyMatrix *= transform3D::RotateOY(RADIANS(angle));

    // BASE PLATE
    glm::mat4 baseModel = wagonBodyMatrix;
    baseModel *= transform3D::Translate(BASE_X_OFFSET, BASE_Y_OFFSET, BASE_Z_OFFSET);
    baseModel *= transform3D::Scale(BASE_LENGTH, BASE_HEIGHT, BASE_WIDTH);
    RenderMeshCustom(meshes.at("trainBase"), shaders.at("VertexColor"),
        baseModel, viewMatrix, projectionMatrix);

    // WHEELS
    for (int i = 0; i < NUM_WHEELS_SIDE; i++) {
        float x_offset = WHEEL_X_START + (i * WHEEL_X_SPACING);

        // Wheel Left
        glm::mat4 LwheelModel = wagonBodyMatrix;
        LwheelModel *= transform3D::Translate(x_offset, WHEEL_Y_OFFSET, L_WHEEL_Z_OFFSET);
        LwheelModel *= transform3D::RotateOX(RADIANS(90));
        LwheelModel *= transform3D::Scale(WHEEL_SCALE_R, WHEEL_SCALE_W, WHEEL_SCALE_R);
        RenderMeshCustom(meshes.at("trainWheel"), shaders.at("VertexColor"), LwheelModel, viewMatrix, projectionMatrix);

        // Wheel Right
        glm::mat4 RwheelModel = wagonBodyMatrix;
        RwheelModel *= transform3D::Translate(x_offset, WHEEL_Y_OFFSET, R_WHEEL_Z_OFFSET);
        RwheelModel *= transform3D::RotateOX(RADIANS(90));
        RwheelModel *= transform3D::Scale(WHEEL_SCALE_R, WHEEL_SCALE_W, WHEEL_SCALE_R);
        RenderMeshCustom(meshes.at("trainWheel"), shaders.at("VertexColor"), RwheelModel, viewMatrix, projectionMatrix);
    }

    // CABIN BODY
    glm::mat4 cabinModel = wagonBodyMatrix;
    cabinModel *= transform3D::Translate(CABIN_X_OFFSET, CABIN_Y_OFFSET, CABIN_Z_OFFSET);
    cabinModel *= transform3D::Scale(CABIN_LENGTH, CABIN_HEIGHT, CABIN_WIDTH);
    RenderMeshCustom(meshes.at("trainCabin"), shaders.at("VertexColor"), cabinModel, viewMatrix, projectionMatrix);
}

void Wagon::SetPosition(glm::vec3 position) {
    this->position = position;
}

void Wagon::SetRotation(float angle) {
    this->angle = angle;
}

Wagon::Wagon(glm::vec3 position) : position(position) {}

Wagon::~Wagon()
{
}
