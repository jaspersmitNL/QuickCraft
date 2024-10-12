#pragma once
#include <glm/glm.hpp>


struct BlockVertex {
    glm::vec3 position;
};

struct BlockFace {
    glm::vec3 center;
    uint32_t orientation;
    uint32_t blockID;
};

struct ChunkUniforms {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
};
