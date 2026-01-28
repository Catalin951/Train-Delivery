#include "lab_m1/Train/Rail.h"
#include <iostream>

Rail::Rail(glm::vec3 start, glm::vec3 end, float angleDegrees, RailType type)
{
    this->startPosition = start;
    this->endPosition = end;
    this->rotationAngleDeg = angleDegrees;
    this->rotationAngleRads = glm::radians(angleDegrees);
    this->length = glm::distance(start, end);
    this->type = type;
    twinRail = nullptr;
    destination = nullptr;
}

Rail::~Rail() {
}