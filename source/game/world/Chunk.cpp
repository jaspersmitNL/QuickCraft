#include "Chunk.hpp"
#include "core/Utils.hpp"
#include "game/MiniCraft.hpp"
#include "game/FastNoise.hpp"
#include "render/WorldRenderer.hpp"
#include <chrono>


auto NOW = []() {
    return std::chrono::high_resolution_clock::now();
};

auto TIME = [](auto start, auto end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
};


static FastNoise *heightNoise;

uint32_t Chunk::GetBlock(int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) {
        return 0;
    }
    uint32_t idx = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT;
    return m_Blocks[idx];
}

void Chunk::SetBlock(int x, int y, int z, uint32_t block) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_SIZE) {
        return;
    }
    uint32_t idx = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_HEIGHT;
    m_Blocks[idx] = block;
}


uint32_t GetBlockID(glm::vec3 pos, float height) {



    if (pos.y == height) return 1; //grass
    if (pos.y == height - 1) return 3; //dirt
    if (pos.y < height - 1) return 5; //stone
    if (pos.y == 0.0f) return 6; //bedrock


    return 0;
}

void Chunk::Generate() {
    auto start = NOW();

    auto seed = 1234;

    heightNoise = new FastNoise(seed);
    //minecraft like terrain generation
    heightNoise->SetNoiseType(FastNoise::SimplexFractal);
    heightNoise->SetFractalOctaves(4);
    heightNoise->SetFractalLacunarity(2.0);
    heightNoise->SetFractalGain(0.7); // Increase gain for smoother terrain
    heightNoise->SetFrequency(0.005); // Decrease frequency for smoother terrain


    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            float height = heightNoise->GetNoise(x + m_Position.x * CHUNK_SIZE, z + m_Position.z * CHUNK_SIZE);
            height = (height + 1.0f) / 2.0f;
            height *= CHUNK_HEIGHT / 2;
            height = std::floor(height);
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                if (y <= height) {
                    SetBlock(x, y, z, GetBlockID({x, y, z}, height));
                }
            }
        }
    }
    auto end = NOW();
    printf("Generated chunk at position: %d, %d in %d ms\n", m_Position.x, m_Position.z, TIME(start, end));
}


enum FaceOrientation: uint32_t {
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5,
};


void Chunk::BuildMesh() {
    m_Faces.clear();

    if (m_InstanceBuffer) {
        printf("Destroying existing buffer\n");
        m_InstanceBuffer.Destroy();
        m_UniformBuffer.Destroy();
    }

    m_Faces.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT * 6);
    auto start = NOW();


    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                uint32_t blockID = GetBlock(x, y, z);
                if (blockID == 0) continue;


                auto GetTextureID = [&](uint32_t blockID, FaceOrientation face) -> uint32_t {

                    switch (blockID) {
                        case 1:
                            if (face == TOP) return 1; //grass top
                            if (face == BOTTOM) return 3; //dirt
                            return 2;
                        default:
                            return blockID;
                    }
                };

                auto AddFace = [&](glm::vec3 center, FaceOrientation orientation, uint32_t blockID) {



                    m_Faces.push_back({
                        .center = center,
                        .orientation = orientation,
                        .blockID = GetTextureID(blockID, orientation),
                    });
                };


                if (GetBlock(x, y, z + 1) == 0) {
                    AddFace(glm::vec3(x, y, z), FRONT, blockID);
                }
                if (GetBlock(x, y, z - 1) == 0) {
                    AddFace(glm::vec3(x, y, z), BACK, blockID);
                }
                if (GetBlock(x + 1, y, z) == 0) {
                    AddFace(glm::vec3(x, y, z), LEFT, blockID);
                }
                if (GetBlock(x - 1, y, z) == 0) {
                    AddFace(glm::vec3(x, y, z), RIGHT, blockID);
                }
                if (GetBlock(x, y + 1, z) == 0) {
                    AddFace(glm::vec3(x, y, z), TOP, blockID);
                }
                if (GetBlock(x, y - 1, z) == 0) {
                    AddFace(glm::vec3(x, y, z), BOTTOM, blockID);
                }
            }
        }
    }


    m_InstanceBuffer = Core::CreateBufferFromData(MiniCraft::Get()->m_RenderContext->m_Device,
                                                  wgpu::BufferUsage::Vertex, m_Faces.data(),
                                                  sizeof(BlockFace) * m_Faces.size());

    m_UniformBuffer = Core::CreateBufferFromData(MiniCraft::Get()->m_RenderContext->m_Device,
                                                 wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst, &m_Uniforms,
                                                 sizeof(ChunkUniforms));

    auto &worldRenderer = MiniCraft::Get()->m_WorldRenderer;

    wgpu::BindGroupDescriptor bindGroupDescriptor{};
    bindGroupDescriptor.layout = worldRenderer->m_Pipeline.GetBindGroupLayout(0);
    std::vector<wgpu::BindGroupEntry> bindGroupEntries = {
        {
            .binding = 0,
            .buffer = m_UniformBuffer,
        },
        {
            .binding = 1,
            .sampler = worldRenderer->m_Sampler,
        },
        {
            .binding = 2,
            .textureView = worldRenderer->m_Texture.CreateView()
        }
    };

    bindGroupDescriptor.entryCount = bindGroupEntries.size();
    bindGroupDescriptor.entries = bindGroupEntries.data();

    m_BindGroup = MiniCraft::Get()->m_RenderContext->m_Device.CreateBindGroup(&bindGroupDescriptor);


    m_VertexCount = m_Faces.size();


    auto end = NOW();
    printf("Built mesh for chunk at position: %d, %d (%d) in %d ms\n", m_Position.x, m_Position.z, m_VertexCount,
           TIME(start, end));

    m_IsReady = true;
}
