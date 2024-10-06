#pragma once
#include <glm/glm.hpp>
#include <webgpu/webgpu_cpp.h>
#include "game/Vertex.hpp"
#include "core/Context.hpp"
#include "game/Uniforms.hpp"
#include "game/Pipelines.hpp"

#define CHUNK_SIZE 16


class Camera;

class Chunk {
public:
    bool m_IsReady = false;

    Chunk(glm::vec3 chunkPosition);

    void GenerateChunk();

    void BuildMesh(Core::Context &ctx, Pipelines& pipelines, wgpu::Sampler& sampler, wgpu::Texture& texture);


    void Render(Core::Context &ctx, Pipelines &pipelines, wgpu::CommandEncoder &encoder,
                wgpu::SurfaceTexture &surfaceTexture, Camera& camera);

public:
    glm::vec3 m_ChunkPosition;
    std::vector<unsigned int> m_ChunkData;
    std::vector<Vertex> m_Vertices;

    ChunkUniforms m_ChunkUniforms;

    wgpu::Buffer m_VertexBuffer = nullptr;
    wgpu::Buffer m_UniformBuffer = nullptr;
    wgpu::BindGroup m_BindGroup = nullptr;
};
