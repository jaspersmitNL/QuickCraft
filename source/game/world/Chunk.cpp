#include <glm/gtc/matrix_transform.hpp>
#include "Chunk.hpp"
#include "core/Utils.hpp"
#include "game/Camera.hpp"
#include "game/Uniforms.hpp"


std::vector<Vertex> vertices2 = {


    //BACK
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left

    //FRONT
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}}, // b-left

    //LEFT
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}}, // b-right
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left

    //RIGHT
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}}, // t-left
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left

    //TOP
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left

    //BOTTOM
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left


};
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

void Chunk::GenerateChunk() {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int y = 1;
            int idx = x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE;
            m_ChunkData[idx] = 1;
        }
    }
}

void Chunk::BuildMesh(Core::Context& ctx, Pipelines& pipelines, wgpu::Sampler& sampler, wgpu::Texture& texture) {


    for (int x = -8; x < 8; x++) {
        for (int y = -8; y < 8; y++)
            for (int z = -8; z < 8; z++) {

                uint32_t blockID = (x + y + z) % 2 == 0;


                auto verts = vertices2;
                for (auto &v: verts) {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                    glm::vec3 pos(model * glm::vec4(v.pos, 1.0f));

                    m_Vertices.push_back({pos, v.uv, blockID});

                }
            }
    }


    if (m_VertexBuffer) {
        m_VertexBuffer.Destroy();
        m_UniformBuffer.Destroy();
    }

    m_VertexBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
    m_UniformBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst, &m_ChunkUniforms, sizeof(ChunkUniforms));

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
    wgpu::SurfaceTexture &surfaceTexture, Camera& camera) {


    m_ChunkUniforms.uProjection = camera.GetProjection();
    m_ChunkUniforms.uView = camera.GetView();
    m_ChunkUniforms.uModel = glm::mat4(1.0f);
    m_ChunkUniforms.uModel = glm::translate(m_ChunkUniforms.uModel, m_ChunkPosition * (float) CHUNK_SIZE);
    ctx.m_Queue.WriteBuffer(m_UniformBuffer, 0, &m_ChunkUniforms, sizeof(ChunkUniforms));



    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store,
        // .clearValue = wgpu::Color{0.0f, 0.0f, 0.0f, 1.0f},
    };

    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = pipelines.m_DepthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Clear,
        .depthStoreOp = wgpu::StoreOp::Store,
        .depthClearValue = 1.0f,
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
