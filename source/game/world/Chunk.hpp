#pragma once
#include "Structs.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <webgpu/webgpu_cpp.h>


#define CHUNK_SIZE 64



class Chunk {
public:
    glm::vec3 m_Position;

    Chunk(glm::vec3 pos): m_Position(pos) {
        m_Blocks.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
    }

    uint32_t GetBlock(int x, int y, int z);
    void SetBlock(int x, int y, int z, uint32_t block);

    void Generate();
    void BuildMesh();




    inline static std::vector<BlockVertex> blockVertices = {
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 1.0f)}, // bottom left
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 1.0f)}, // bottom right
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)}, // top right
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)}, // top right
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f)}, // top left
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 1.0f)}, // bottom left
    };

    inline static std::vector<BlockFace> blockFaces = {
        {{0.0f, 0.0f, 0.0f}, 0}, //front
        {{0.0f, 0.0f, 0.0f}, 1}, // back
        {{0.0f, 0.0f, 0.0f}, 2}, //left
        {{0.0f, 0.0f, 0.0f}, 3}, //right
        {{0.0f, 0.0f, 0.0f}, 4}, //top
        {{0.0f, 0.0f, 0.0f}, 5}, //bottom
    };


    wgpu::Buffer m_InstanceBuffer = nullptr;
    wgpu::Buffer m_UniformBuffer = nullptr;
    wgpu::BindGroup m_BindGroup = nullptr;

    ChunkUniforms m_Uniforms{};

    uint32_t m_VertexCount = 0;

    bool m_IsReady = false;


private:
    std::vector<uint32_t> m_Blocks;
    std::vector<BlockFace> m_Faces;
};
