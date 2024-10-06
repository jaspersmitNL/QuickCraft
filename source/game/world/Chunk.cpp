#include <glm/gtc/matrix_transform.hpp>
#include "Chunk.hpp"
#include "core/Utils.hpp"
#include "game/Camera.hpp"
#include "game/Uniforms.hpp"
#include "glm/gtc/noise.hpp"
#include "FastNoiseLite.hpp"


enum Face {
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5
};

std::vector<Vertex> GetFaceVerts(Face face, unsigned int id) {
    std::vector<Vertex> vertices;

    switch (face) {
        case BACK: {
            vertices = {
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
                {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, id}, // b-right
                {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, id}, // t-left
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
            };
            break;
        }
        case FRONT: {
            vertices = {
                {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, id}, // b-left
                {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, id}, // b-right
                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, id}, // t-left
                {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, id}, // b-left
            };
            break;
        }
        case LEFT: {
            vertices = {
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
                {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, id}, // b-right
                {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, id}, // t-left
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
            };
            break;
        }
        case RIGHT: {
            vertices = {
                {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
                {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, id}, // b-right
                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, id}, // t-left
                {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
            };
            break;
        }
        case TOP: {
            vertices = {
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
                {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}, id}, // b-right
                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, id}, // t-left
                {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
            };
            break;
        }
        case BOTTOM: {
            vertices = {
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
                {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, id}, // b-right
                {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}, id}, // t-right
                {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, id}, // t-left
                {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, id}, // b-left
            };
            break;
        }
    }


    return vertices;
}


Chunk::Chunk(glm::vec3 chunkPosition) {
    m_ChunkPosition = chunkPosition;
    m_ChunkData.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
}

unsigned int Chunk::GetBlock(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return 0;
    }
    int idx = x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE;
    return m_ChunkData[idx];
}

void Chunk::GenerateChunk() {
    FastNoiseLite noise;

    noise.SetSeed(123);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);
    noise.SetFrequency(0.1f);


    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {

            //pick a random y

            // make a 'perlins noise' using FastNoiseLite from 1 to CHUNK_SIZE
            int y2 = (int) (noise.GetNoise((float) x, (float) z) * CHUNK_SIZE / 2 + CHUNK_SIZE / 2);

            for (int y = 0; y < y2; y++) {
                int idx = x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE;

                uint32_t blockID = ((x + y + z) % 2) + 1;
                m_ChunkData[idx] = blockID;


            }
        }
    }
}

void Chunk::BuildMesh(Core::Context &ctx, Pipelines &pipelines, wgpu::Sampler &sampler, wgpu::Texture &texture) {
    auto addFace = [&](Face face, int x, int y, int z, unsigned int id) {
        auto verts = GetFaceVerts(face, id);
        for (auto &v: verts) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
            glm::vec3 pos(model * glm::vec4(v.pos, 1.0f));
            m_Vertices.push_back({pos, v.uv, id});
        }
    };


    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int z = 0; z < CHUNK_SIZE; z++) {
                uint32_t blockID = GetBlock(x, y, z);

                if (blockID == 0) {
                    continue;
                }


                if (GetBlock(x, y, z + 1) == 0) {
                    addFace(Face::FRONT, x, y, z, blockID);
                }

                if (GetBlock(x, y, z - 1) == 0) {
                    addFace(Face::BACK, x, y, z, blockID);
                }

                if (GetBlock(x - 1, y, z) == 0) {
                    addFace(Face::LEFT, x, y, z, blockID);
                }

                if (GetBlock(x + 1, y, z) == 0) {
                    addFace(Face::RIGHT, x, y, z, blockID);
                }

                if (GetBlock(x, y + 1, z) == 0) {
                    addFace(Face::TOP, x, y, z, blockID);
                }

                if (GetBlock(x, y - 1, z) == 0) {
                    addFace(Face::BOTTOM, x, y, z, blockID);
                }

            }
    }


    if (m_VertexBuffer) {
        m_VertexBuffer.Destroy();
        m_UniformBuffer.Destroy();
    }

    m_VertexBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, m_Vertices.data(),
                                                m_Vertices.size() * sizeof(Vertex));
    m_UniformBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
                                                 &m_ChunkUniforms, sizeof(ChunkUniforms));

    wgpu::BindGroupEntry bindGroupEntries[3] = {
        {
            .binding = 0,
            .buffer = m_UniformBuffer,
            .offset = 0,
            .size = sizeof(ChunkUniforms),
        },
        {
            .binding = 1,
            .sampler = sampler,
        },
        {
            .binding = 2,
            .textureView = texture.CreateView()
        }
    };

    wgpu::BindGroupDescriptor bindGroupDescriptor{
        .label = "Uniform Bind Group",
        .layout = pipelines.m_ChunkPipeline.GetBindGroupLayout(0),
        .entryCount = 3,
        .entries = bindGroupEntries,
    };

    m_BindGroup = ctx.m_Device.CreateBindGroup(&bindGroupDescriptor);
}

void Chunk::Render(Core::Context &ctx, Pipelines &pipelines, wgpu::CommandEncoder &encoder,
                   wgpu::SurfaceTexture &surfaceTexture, Camera &camera) {
    m_ChunkUniforms.uProjection = camera.GetProjection();
    m_ChunkUniforms.uView = camera.GetView();
    m_ChunkUniforms.uModel = glm::mat4(1.0f);
    m_ChunkUniforms.uModel = glm::translate(m_ChunkUniforms.uModel, m_ChunkPosition * (float) CHUNK_SIZE);
    ctx.m_Queue.WriteBuffer(m_UniformBuffer, 0, &m_ChunkUniforms, sizeof(ChunkUniforms));


    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store
    };

    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = pipelines.m_DepthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Load,
        .depthStoreOp = wgpu::StoreOp::Store
    };


    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachment,
        .depthStencilAttachment = &renderPassDepthStencilAttachment,
    };

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
    pass.SetPipeline(pipelines.m_ChunkPipeline);
    pass.SetBindGroup(0, m_BindGroup);
    pass.SetVertexBuffer(0, m_VertexBuffer);
    pass.Draw(m_Vertices.size(), 1, 0, 0);
    pass.End();
}
