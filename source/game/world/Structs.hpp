#pragma once
#include <glm/glm.hpp>


struct BlockVertex {
    glm::vec3 position;
    glm::vec2 uv;
};

struct BlockFace {
    glm::vec3 center;
    uint32_t orientation;
    uint32_t blockID;
};

struct Uniforms {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 inverseProjection;
    glm::mat4 inverseView;
    glm::vec4 cameraPosition;
    float fogNear = 190.0f;
    float fogFar = 200.0f;
    float padding[2];
};
