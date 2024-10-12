//
// Created by jasper on 8-10-2024.
//

#include "WorldRenderer.hpp"
#include "core/Utils.hpp"
#include "core/builder/RenderPipelineBuilder.hpp"
#include "game/MiniCraft.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "GLFW/glfw3.h"

void WorldRenderer::Init() {
    auto &ctx = MiniCraft::Get()->m_RenderContext;

    MiniCraft::Get()->m_Camera->SetPosition(glm::vec3(0, 25, 0));

    m_VertexBuffer = Core::CreateBufferFromData(ctx->m_Device, wgpu::BufferUsage::Vertex, Chunk::blockVertices.data(),
                                                sizeof(BlockVertex) * Chunk::blockVertices.size());


    m_Shader = Core::LoadShaderFromFile(ctx->m_Device, "../res/world.wgsl", "World Shader");

    // wgpu::ColorTargetState colorTargetState{
    //     .format = ctx->m_SurfaceTextureFormat,
    // };
    //
    // wgpu::FragmentState fragmentState{
    //     .module = m_Shader,
    //     .entryPoint = "fs_main",
    //     .targetCount = 1,
    //     .targets = &colorTargetState,
    // };
    //
    // wgpu::VertexAttribute vertexAttributes[1] = {
    //     {
    //         .format = wgpu::VertexFormat::Float32x3,
    //         .offset = 0,
    //         .shaderLocation = 0,
    //     }
    // };
    // wgpu::VertexAttribute instanceAttributes[3] = {
    //     {
    //         .format = wgpu::VertexFormat::Float32x3,
    //         .offset = offsetof(BlockFace, center),
    //         .shaderLocation = 1,
    //     },
    //     {
    //         .format = wgpu::VertexFormat::Uint32,
    //         .offset = offsetof(BlockFace, orientation),
    //         .shaderLocation = 2,
    //     },
    //     {
    //         .format = wgpu::VertexFormat::Uint32,
    //         .offset = offsetof(BlockFace, blockID),
    //         .shaderLocation = 3,
    //     }
    // };
    // wgpu::VertexBufferLayout bufferLayouts[2] = {
    //     {
    //         .arrayStride = sizeof(BlockVertex),
    //         .stepMode = wgpu::VertexStepMode::Vertex,
    //         .attributeCount = 1,
    //         .attributes = vertexAttributes,
    //     },
    //     {
    //         .arrayStride = sizeof(BlockFace),
    //         .stepMode = wgpu::VertexStepMode::Instance,
    //         .attributeCount = 3,
    //         .attributes = instanceAttributes,
    //     }
    // };
    //
    // wgpu::VertexState vertexState{
    //     .module = m_Shader,
    //     .entryPoint = "vs_main",
    //     .bufferCount = 2,
    //     .buffers = bufferLayouts,
    // };
    //
    // wgpu::DepthStencilState depthStencilState{
    //     .format = wgpu::TextureFormat::Depth24Plus,
    //     .depthWriteEnabled = {true},
    //     .depthCompare = wgpu::CompareFunction::Less,
    // };
    //
    // wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
    //     .label = "World Render Pipeline",
    //     .vertex = vertexState,
    //     .primitive = {
    //         .topology = wgpu::PrimitiveTopology::TriangleList,
    //     },
    //     .depthStencil = &depthStencilState,
    //     .fragment = &fragmentState,
    // };
    //
    // m_Pipeline = ctx->m_Device.CreateRenderPipeline(&renderPipelineDescriptor);


    Core::RenderPipelineBuilder builder(ctx);
    m_Pipeline = builder
            .SetVertexEntryPoint("vs_main")
            .SetFragmentEntryPoint("fs_main")
            .SetShaderModule(m_Shader)
            .AddVertexBufferLayout({
                {
                    .format = wgpu::VertexFormat::Float32x3,
                    .offset = 0,
                    .shaderLocation = 0,
                }
            }, sizeof(BlockVertex), wgpu::VertexStepMode::Vertex)
            .AddVertexBufferLayout({
                {
                    .format = wgpu::VertexFormat::Float32x3,
                    .offset = offsetof(BlockFace, center),
                    .shaderLocation = 1,
                },
                {
                    .format = wgpu::VertexFormat::Uint32,
                    .offset = offsetof(BlockFace, orientation),
                    .shaderLocation = 2,
                },
                {
                    .format = wgpu::VertexFormat::Uint32,
                    .offset = offsetof(BlockFace, blockID),
                    .shaderLocation = 3,
                }
            }, sizeof(BlockFace), wgpu::VertexStepMode::Instance)
            .EnableDepthStencil(true)
            .Build();

    printf("WorldRenderer initialized\n");
}

void WorldRenderer::Render(wgpu::CommandEncoder &commandEncoder, wgpu::SurfaceTexture &surfaceTexture) {
    auto &ctx = MiniCraft::Get()->m_RenderContext;
    auto &camera = MiniCraft::Get()->m_Camera;

    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {0.0f, 0.0f, 0.0f, 1.0f},
    };

    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = ctx->m_DepthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Load,
        .depthStoreOp = wgpu::StoreOp::Store
    };

    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "World Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachment,
        .depthStencilAttachment = &renderPassDepthStencilAttachment,
    };

    wgpu::RenderPassEncoder pass = commandEncoder.BeginRenderPass(&renderPassDescriptor);

    pass.SetPipeline(m_Pipeline);
    pass.SetVertexBuffer(0, m_VertexBuffer);

    for(auto& chunk : m_World->m_Chunks) {
        if(!chunk->m_IsReady) continue;

        chunk->m_Uniforms.projection = camera->GetProjection();
        chunk->m_Uniforms.view = camera->GetView();
        chunk->m_Uniforms.model = glm::translate(glm::mat4(1.0f), glm::vec3(
            chunk->m_Position.x * CHUNK_SIZE,
            chunk->m_Position.y * CHUNK_SIZE,
            chunk->m_Position.z * CHUNK_SIZE
        ));




        pass.SetBindGroup(0, chunk->m_BindGroup);
        ctx->m_Device.GetQueue().WriteBuffer(chunk->m_UniformBuffer, 0, &chunk->m_Uniforms, sizeof(ChunkUniforms));
        pass.SetVertexBuffer(1, chunk->m_InstanceBuffer);
        pass.Draw(6, chunk->m_VertexCount, 0, 0);
    }

    pass.End();


}
