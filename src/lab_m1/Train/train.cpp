#include "lab_m1/Train/Train.h"
#include "lab_m1/Train/rail.h"
#include "utils/glm_utils.h"
#include "lab_m1/Train/transform3D.h"
#include "lab_m1/Train/renderable.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

void Train::Render(const std::unordered_map<std::string, Mesh*>& meshes,
    const std::unordered_map<std::string, Shader*>& shaders,
    const glm::mat4& viewMatrix,
    const glm::mat4& projectionMatrix)
{   
    // 1. DEFINE THE "BODY MATRIX"
    // This represents the Train's center point and its facing direction.
    // Order: Translate to World Position -> Rotate in Place
    glm::mat4 trainBodyMatrix = glm::mat4(1);
    trainBodyMatrix *= transform3D::Translate(position.x, position.y, position.z);
    trainBodyMatrix *= transform3D::RotateOY(RADIANS(angle)); // Rotate 90 degrees Right

    // BASE PLATE
    glm::mat4 baseModel = trainBodyMatrix;
    baseModel *= transform3D::Translate(BASE_X_OFFSET, BASE_Y_OFFSET, BASE_Z_OFFSET);
    baseModel *= transform3D::Scale(BASE_LENGTH, BASE_HEIGHT, BASE_WIDTH);
    RenderMeshCustom(meshes.at("trainBase"), shaders.at("VertexColor"),
        baseModel, viewMatrix, projectionMatrix);

    // WHEELS
    for (int i = 0; i < NUM_WHEELS_SIDE; i++) {
        float x_offset = WHEEL_X_START + (i * WHEEL_X_SPACING);

        // wheel left
        glm::mat4 LwheelModel = trainBodyMatrix;
        LwheelModel *= transform3D::Translate(x_offset, WHEEL_Y_OFFSET, L_WHEEL_Z_OFFSET);
        LwheelModel *= transform3D::RotateOX(RADIANS(90));
        LwheelModel *= transform3D::Scale(WHEEL_SCALE_R, WHEEL_SCALE_W, WHEEL_SCALE_R);
        RenderMeshCustom(meshes.at("trainWheel"), shaders.at("VertexColor"), LwheelModel, viewMatrix, projectionMatrix);

        // wheel right
        glm::mat4 RwheelModel = trainBodyMatrix;
        RwheelModel *= transform3D::Translate(x_offset, WHEEL_Y_OFFSET, R_WHEEL_Z_OFFSET);
        RwheelModel *= transform3D::RotateOX(RADIANS(90));
        RwheelModel *= transform3D::Scale(WHEEL_SCALE_R, WHEEL_SCALE_W, WHEEL_SCALE_R);
        RenderMeshCustom(meshes.at("trainWheel"), shaders.at("VertexColor"), RwheelModel, viewMatrix, projectionMatrix);
    }

    // --- CABIN BODY ---
    glm::mat4 cabinModel = trainBodyMatrix;
    cabinModel *= transform3D::Translate(CABIN_X_OFFSET, CABIN_Y_OFFSET, CABIN_Z_OFFSET);
    cabinModel *= transform3D::Scale(CABIN_LENGTH, CABIN_HEIGHT, CABIN_WIDTH);
    RenderMeshCustom(meshes.at("trainCabin"), shaders.at("VertexColor"), cabinModel, viewMatrix, projectionMatrix);

    // --- ENGINE AREA ---
    glm::mat4 engineModel = trainBodyMatrix;
    engineModel *= transform3D::Translate(ENGINE_X_OFFSET, ENGINE_Y_OFFSET, ENGINE_Z_OFFSET);
    engineModel *= transform3D::RotateOZ(RADIANS(90));
    engineModel *= transform3D::Scale(ENGINE_SCALE_R, ENGINE_SCALE_L, ENGINE_SCALE_R);
    RenderMeshCustom(meshes.at("trainEngine"), shaders.at("VertexColor"), engineModel, viewMatrix, projectionMatrix);

    // --- INNER CYLINDER ---
    glm::mat4 innerModel = trainBodyMatrix;
    innerModel *= transform3D::Translate(INNER_ENGINE_X_OFFSET, INNER_ENGINE_Y_OFFSET, 0);
    innerModel *= transform3D::RotateOZ(RADIANS(90));
    innerModel *= transform3D::Scale(INNER_SCALE_R, INNER_SCALE_L, INNER_SCALE_R);
    RenderMeshCustom(meshes.at("trainInnerEngine"), shaders.at("VertexColor"), innerModel, viewMatrix, projectionMatrix);

    // Render Wagons
    for (int i = 0; i < wagons.size(); i++) {
        glm::vec3 localOffset = glm::vec3(WAGON_OFFSET_X * (i + 1), 0, 0);
        glm::vec4 worldPos4 = trainBodyMatrix * glm::vec4(localOffset, 1.0f);
        glm::vec3 worldPos = glm::vec3(worldPos4);
        wagons[i]->SetPosition(worldPos);
        wagons[i]->SetRotation(angle);
        wagons[i]->Render(meshes, shaders, viewMatrix, projectionMatrix);
    }
}

void Train::Update(float deltaTimeSeconds) {
    if (currentRail == nullptr) return;

    if (isWaitingForPath)
        return;
    distanceTraveled += speed * deltaTimeSeconds;
    glm::vec3 railDir = glm::normalize(currentRail->endPosition - currentRail->startPosition);
    // Check end of rail
    if (distanceTraveled >= currentRail->length) {
        Intersection* arrivalNode = currentRail->destination;
        std::vector<Rail*> validOptions;
        if (arrivalNode != nullptr) {
            for (auto r : arrivalNode->outgoingRails) {
                if (r != currentRail->twinRail) {
                    validOptions.push_back(r);
                }
            }
        }
        // Decision
        if (validOptions.empty()) {
            // End of line
            // should go back
            if (!arrivalNode->outgoingRails.empty()) {
                // take way back (the same rail but reversed)
                Rail* returnRail = arrivalNode->outgoingRails[0];
                currentRail = returnRail;
                distanceTraveled = 0;
                float excess = distanceTraveled - currentRail->length;
                position = currentRail->startPosition;
                // Rotate train
                glm::vec3 railDir = glm::normalize(currentRail->endPosition - currentRail->startPosition);
                float angleRad = atan2(railDir.x, railDir.z);
                angle = glm::degrees(angleRad) - 90;
            } else {
                // Shouldnt be here
                distanceTraveled = currentRail->length;
                speed = 0;
            }
        } else if (validOptions.size() == 1) {
            // 1 option
            float excess = distanceTraveled - currentRail->length;
            currentRail = validOptions[0];
            distanceTraveled = excess;
            railDir = glm::normalize(currentRail->endPosition - currentRail->startPosition);
            float angleRad = atan2(railDir.x, railDir.z);
            angle = glm::degrees(angleRad) - 90;
        } else {
            // Actual intersection
            isWaitingForPath = true;
            distanceTraveled = currentRail->length;
            position = currentRail->endPosition;
            return;
        }
    }
    position = currentRail->startPosition + (railDir * distanceTraveled);
}

float Train::GetAngleDiff(glm::vec3 dirA, glm::vec3 dirB) {
    float angleA = atan2(dirA.x, dirA.z);
    float angleB = atan2(dirB.x, dirB.z);
    float diff = angleB - angleA;
    // Normalization
    while (diff > 3.14159f) diff -= 6.28318f;
    while (diff < -3.14159f) diff += 6.28318f;
    return diff;
}

void Train::ResolveIntersection(int directionCode) {
    if (!isWaitingForPath || currentRail == nullptr) return;

    Intersection* node = currentRail->destination;
    std::vector<Rail*> options;

    for (auto r : node->outgoingRails) {
        if (r != currentRail->twinRail) options.push_back(r);
    }
    if (options.empty()) return;

    glm::vec3 currentDir = glm::normalize(currentRail->endPosition - currentRail->startPosition);
    Rail* selectedRail = nullptr;

    float minAngle = 0.0f;
    float maxAngle = 0.0f;

    switch (directionCode) {
    case DIR_FORWARD:
        minAngle = -25.0f; maxAngle = 25.0f;
        break;

    case DIR_SOFT_LEFT: // Q - Soft Left ~45
        minAngle = 25.0f; maxAngle = 70.0f;
        break;
    case DIR_SOFT_RIGHT: // E - Soft Right~ -45
        minAngle = -70.0f; maxAngle = -25.0f;
        break;

    case DIR_HARD_LEFT: // A - Hard Left (Standard 90)
        minAngle = 70.0f; maxAngle = 115.0f;
        break;
    case DIR_HARD_RIGHT: // D - Hard Right (Standard -90)
        minAngle = -115.0f; maxAngle = -70.0f;
        break;

    case DIR_SHARP_LEFT: // Z - Sharp Left  ~135
        minAngle = 115.0f; maxAngle = 180.0f;
        break;
    case DIR_SHARP_RIGHT: // C - Sharp Right ~ -135
        minAngle = -180.0f; maxAngle = -115.0f;
        break;
    }

    // Look for the rail
    for (auto r : options) {
        glm::vec3 nextDir = glm::normalize(r->endPosition - r->startPosition);
        float diffDeg = glm::degrees(GetAngleDiff(currentDir, nextDir));

        if (diffDeg >= minAngle && diffDeg <= maxAngle) {
            selectedRail = r;
            break;
        }
    }

    // FALLBACKS (for smoother gameplay)

    // 1. if you press w but no straight rail, try Q E
    if (!selectedRail && directionCode == DIR_FORWARD) {
        for (auto r : options) {
            float diff = glm::degrees(GetAngleDiff(currentDir, glm::normalize(r->endPosition - r->startPosition)));
            if (abs(diff) < 50.0f) { selectedRail = r; break; }
        }
    }

    // 2. if you press A/D but you only have Z/C
    if (!selectedRail && abs(directionCode) >= 2) {
        bool searchLeft = (directionCode < 0);
        for (auto r : options) {
            float diff = glm::degrees(GetAngleDiff(currentDir, glm::normalize(r->endPosition - r->startPosition)));
            if (searchLeft && diff > 45.0f) { selectedRail = r; break; }
            if (!searchLeft && diff < -45.0f) { selectedRail = r; break; }
        }
    }

    if (selectedRail) {
        currentRail = selectedRail;
        distanceTraveled = 0;
        position = currentRail->startPosition;

        glm::vec3 newDir = glm::normalize(currentRail->endPosition - currentRail->startPosition);
        float angleRad = atan2(newDir.x, newDir.z);
        angle = glm::degrees(angleRad) - 90;

        isWaitingForPath = false;
    }
}

void Train::SetRail(Rail* rail) {
    currentRail = rail;
    distanceTraveled = 0.0f;
    position = rail->startPosition;
}

void Train::SetSpeed(float s) {
    speed = s;
}

Train::Train(glm::vec3 position, float angleDeg) : position(position), angle(angleDeg)
{
    currentRail = nullptr;
    speed = 0.0f;
    distanceTraveled = 0.0f;
    for (int i = 0; i < totalWagons; i++) {
        wagons.push_back(new Wagon(glm::vec3(position.x + WAGON_OFFSET_X, position.y, position.z)));
    }
}

Train::~Train()
{
}
