#pragma once
#include "utils/glm_utils.h"
#include <vector>

class Intersection;

enum RailType {
    RAIL_NORMAL, // plain
    RAIL_BRIDGE, // water
    RAIL_TUNNEL  // mountain
};

class Rail {
public:
    Rail(glm::vec3 start, glm::vec3 end, float angleDegrees, RailType type = RAIL_NORMAL);
    ~Rail();    
    Rail* twinRail; // this is the inverse of our rail, coming in to our intersection logically
    Intersection* destination;
    glm::vec3 startPosition;
    glm::vec3 endPosition;
    std::vector<Rail*> nextRails;
    bool isVisual; // this tells if a rail is just logical (the inverse) or must be shown
    float length;
    float rotationAngleRads;
    float rotationAngleDeg;
    RailType type;
};

class Intersection {
public:
    int id;
    glm::vec3 position;
    std::vector<Rail*> outgoingRails;

    Intersection(int id, glm::vec3 pos) : id(id), position(pos) {}
};