#include "Chunk.hpp"
#include "core/Utils.hpp"
#include "game/MiniCraft.hpp"
#include <chrono>

#include "render/WorldRenderer.hpp"


auto NOW = []() {
    return std::chrono::high_resolution_clock::now();
};

auto TIME = [](auto start, auto end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
};

uint32_t Chunk::GetBlock(int x, int y, int z) {
    const int idx = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return 0;
    }
    return m_Blocks[idx];
}

void Chunk::SetBlock(int x, int y, int z, uint32_t block) {
    const int idx = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return;
    }
    m_Blocks[idx] = block;
}

void Chunk::Generate() {
    auto start = NOW();
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                uint32_t blockID = (x + y + z) % 2 == 0 ? 1 : 2;
                SetBlock(x, y, z, blockID);
            }
        }
    }
    auto end = NOW();
    printf("Generated chunk at position: %d, %d in %d ms\n", m_Position.x ,m_Position.z, TIME(start, end));
}

void Chunk::BuildMesh() {
    auto start = NOW();



    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                uint32_t blockID = GetBlock(x, y, z);
                if(blockID == 0) continue;


                if(GetBlock(x, y, z+1) == 0) {
                    m_Faces.push_back({
                        .center = glm::vec3(x, y, z),
                        .orientation = 0,
                        .blockID = blockID
                    });
                }
                if(GetBlock(x, y, z-1) == 0) {
                    m_Faces.push_back({
                        .center = glm::vec3(x, y, z),
                        .orientation = 1,
                        .blockID = blockID
                    });
                }
                if(GetBlock(x+1, y, z) == 0) {
                    m_Faces.push_back({
                        .center = glm::vec3(x, y, z),
                        .orientation = 2,
                        .blockID = blockID
                    });
                }
                if(GetBlock(x-1, y, z) == 0) {
                    m_Faces.push_back({
                        .center = glm::vec3(x, y, z),
                        .orientation = 3,
                        .blockID = blockID
                    });
                }
                if(GetBlock(x, y+1, z) == 0) {
                    m_Faces.push_back({
                        .center = glm::vec3(x, y, z),
                        .orientation = 4,
                        .blockID = blockID
                    });
                }
                if(GetBlock(x, y-1, z) == 0) {
                    m_Faces.push_back({
                        .center = glm::vec3(x, y, z),
                        .orientation = 5,
                        .blockID = blockID
                    });
                }
                // for(auto& f : Chunk::blockFaces) {
                //     BlockFace face{};
                //     face.center = glm::vec3(x, y, z);
                //     face.orientation = f.orientation;
                //     face.blockID = blockID;
                //     m_Faces.push_back(face);
                // }
            }
        }
    }


    m_InstanceBuffer = Core::CreateBufferFromData(MiniCraft::Get()->m_RenderContext->m_Device, wgpu::BufferUsage::Vertex, m_Faces.data(),
                                                  sizeof(BlockFace) * m_Faces.size());

    m_UniformBuffer = Core::CreateBufferFromData(MiniCraft::Get()->m_RenderContext->m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst, &m_Uniforms, sizeof(ChunkUniforms));

    wgpu::BindGroupDescriptor bindGroupDescriptor{};
    bindGroupDescriptor.layout = MiniCraft::Get()->m_WorldRenderer->m_Pipeline.GetBindGroupLayout(0);
    bindGroupDescriptor.entryCount = 1;
    wgpu::BindGroupEntry bindGroupEntries[1] = {
        {
            .binding = 0,
            .buffer = m_UniformBuffer,
        },
    };
    bindGroupDescriptor.entries = bindGroupEntries;

    m_BindGroup = MiniCraft::Get()->m_RenderContext->m_Device.CreateBindGroup(&bindGroupDescriptor);


    m_VertexCount = m_Faces.size();



    auto end = NOW();
    printf("Built mesh for chunk at position: %d, %d in %d ms\n", m_Position.x ,m_Position.z, TIME(start, end));

    m_IsReady = true;
}
