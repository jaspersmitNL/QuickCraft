#pragma once
#include "core/Ref.hpp"
#include <glm/glm.hpp>
class Chunk;

struct RayCastResult {
    bool didHit;
    float distance;
    glm::vec3 hitPos;
    glm::vec3 blockPos;
    Ref<Chunk> chunk;
};

class Raycast {
public:
    static RayCastResult Cast(glm::vec3 origin, glm::vec3 direction, float maxDistance);
};


