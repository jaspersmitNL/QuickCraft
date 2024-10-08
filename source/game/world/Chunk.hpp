#pragma once
#include <glm/glm.hpp>
#include <vector>

#define CHUNK_SIZE 16


struct BlockVertex {
    glm::vec3 position;
};

struct BlockFace {
    glm::vec3 center;
    uint32_t orientation;
    uint32_t blockID;
};

struct Uniforms {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
};





class Chunk {
public:
    Chunk(glm::vec3 pos): m_Position(pos) {
        m_Blocks.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
    }

    uint32_t GetBlock(int x, int y, int z);
    void SetBlock(int x, int y, int z, uint32_t block);

    void Generate();


    glm::vec3 m_Position;

    inline static std::vector<BlockVertex> blockVertices = {
        {glm::vec3(-0.5f, -0.5f, 0.5f)}, // bottom left
        {glm::vec3(0.5f, -0.5f, 0.5f)}, // bottom right
        {glm::vec3(0.5f, 0.5f, 0.5f)}, // top right
        {glm::vec3(0.5f, 0.5f, 0.5f)}, // top right
        {glm::vec3(-0.5f, 0.5f, 0.5f)}, // top left
        {glm::vec3(-0.5f, -0.5f, 0.5f)}, // bottom left
    };

    inline static std::vector<BlockFace> blockFaces = {
        {{0.0f, 0.0f, 0.0f}, 0}, //front
        {{0.0f, 0.0f, 0.0f}, 1}, // back
        {{0.0f, 0.0f, 0.0f}, 2}, //left
        {{0.0f, 0.0f, 0.0f}, 3}, //right
        {{0.0f, 0.0f, 0.0f}, 4}, //top
        {{0.0f, 0.0f, 0.0f}, 5}, //bottom
    };

private:
    std::vector<uint32_t> m_Blocks;
};
