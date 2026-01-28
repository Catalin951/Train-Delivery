#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

    class Camera
    {
    public:
        Camera()
        {
            position = glm::vec3(0, 2, 5);
            forward = glm::vec3(0, 0, -1);
            up = glm::vec3(0, 1, 0);
            right = glm::vec3(1, 0, 0);
            distanceToTarget = 2;
        }

        Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            Set(position, center, up);
        }

        ~Camera()
        {
        }

        // Update camera
        void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
        {
            this->position = position;
            forward = glm::normalize(center - position);
            right = glm::cross(forward, up);
            this->up = glm::cross(right, forward);
        }

        void MoveForward(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            position += dir * distance;
        }

        void TranslateForward(float distance)
        {
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, forward.y, forward.z));
            position += dir * distance;
        }

        void TranslateUpward(float distance)
        {
            glm::vec3 dir = glm::normalize(up);
            position += dir * distance;
        }

        void TranslateRight(float distance)
        {
            glm::vec3 dir = normalize(glm::cross(forward, up));
            position += dir * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            glm::vec4 newForward4 = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1);
            forward = glm::normalize(glm::vec3(newForward4));

            glm::vec4 newUp4 = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(up, 1);
            up = glm::normalize(glm::vec3(newUp4));
        }

        void RotateFirstPerson_OY(float angle)
        {
            glm::vec4 newForward4 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
            forward = glm::normalize(glm::vec3(newForward4));

            glm::vec4 newUp4 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(up, 1);
            up = glm::normalize(glm::vec3(newUp4));
            right = glm::cross(forward, up);
        }

        void RotateFirstPerson_OZ(float angle)
        {
            glm::vec4 newUp4 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1)) * glm::vec4(up, 1);
            up = glm::normalize(glm::vec3(newUp4));
            right = glm::cross(up, forward);
        }

        void RotateThirdPerson_OX(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OX(angle);
            TranslateForward(-distanceToTarget);

        }

        void RotateThirdPerson_OY(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OY(angle);
            TranslateForward(-distanceToTarget);
        }

        void RotateThirdPerson_OZ(float angle)
        {
            TranslateForward(distanceToTarget);
            RotateFirstPerson_OZ(angle);
            TranslateForward(-distanceToTarget);
        }

        glm::mat4 GetViewMatrix()
        {
            // Returns the view matrix
            return glm::lookAt(position, position + forward, up);
        }

        glm::vec3 GetTargetPosition()
        {
            return position + forward * distanceToTarget;
        }

        glm::mat4 GetProjectionMatrix() {
            return projectionMatrix;
        }
        void SetProjectionMatrix(glm::mat4 projectionMatrix) {
            this->projectionMatrix = projectionMatrix;
        }

    public:
        glm::mat4 projectionMatrix;
        float distanceToTarget;
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
    };

