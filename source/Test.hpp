#pragma once
#include "game/Vertex.hpp"
#include "game/Pipelines.hpp"
#include "core/Context.hpp"
#include "core/Utils.hpp"
#include "game/Uniforms.hpp"

namespace Test {
    struct InstanceVertex {
        glm::vec3 pos;
    };

    struct CenterBox {
        glm::vec3 center;
    };


    std::vector<InstanceVertex> vertices = {
        {glm::vec3(-0.5f, -0.5f, 0.0f) * 0.5f}, // bottom left
        {glm::vec3(0.5f, -0.5f, 0.0f) * 0.5f}, // bottom right
        {glm::vec3(0.5f, 0.5f, 0.0f) * 0.5f}, // top right

        {glm::vec3(0.5f, 0.5f, 0.0f) * 0.5f}, // top right
        {glm::vec3(-0.5f, 0.5f, 0.0f) * 0.5f}, // top left
        {glm::vec3(-0.5f, -0.5f, 0.0f) * 0.5f}, // bottom left

    };

    std::vector<CenterBox> instances = {
        {{-0.5f, 0.0f, 0.0f}},
        {{0.5f, 0.0f, 0.0f}},
    };

    inline wgpu::ShaderModule shaderModule;
    inline wgpu::Buffer vertexBuffer;
    inline wgpu::Buffer instanceBuffer;
    inline wgpu::RenderPipeline pipeline;

    inline void SetupTest(Core::Context &ctx) {
        shaderModule = Core::LoadShaderFromFile(ctx.m_Device, "../res/instanced.wgsl", "Instanced");

        vertexBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, vertices.data(),
                                                  sizeof(InstanceVertex) * vertices.size());

        instanceBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, instances.data(),
                                                    sizeof(CenterBox) * instances.size());


        wgpu::ColorTargetState colorTargetState{
            .format = ctx.m_SurfaceTextureFormat,
        };

        wgpu::FragmentState fragmentState{
            .module = shaderModule,
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = &colorTargetState,
        };
        wgpu::VertexAttribute attributes[1] = {
            {
                .format = wgpu::VertexFormat::Float32x3,
                .offset = offsetof(InstanceVertex, pos),
                .shaderLocation = 0,
            }
        };

        wgpu::VertexAttribute attributes2[1] = {
            {
                .format = wgpu::VertexFormat::Float32x3,
                .offset = offsetof(CenterBox, center),
                .shaderLocation = 1,
            }
        };


        wgpu::VertexBufferLayout vertexBufferLayouts[2] = {
            {
                .arrayStride = sizeof(InstanceVertex),
                .attributeCount = 1,
                .attributes = attributes,
            },
            {
                .arrayStride = sizeof(CenterBox),
                .stepMode = wgpu::VertexStepMode::Instance,
                .attributeCount = 1,
                .attributes = attributes2,
            }
        };

        wgpu::VertexState vertexState{
            .module = shaderModule,
            .entryPoint = "vs_main",
            .bufferCount = 2,
            .buffers = vertexBufferLayouts,
        };
        wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
            .label = "Test Render Pipeline",
            .vertex = vertexState,
            .primitive = {
                //wireframe
                .topology = wgpu::PrimitiveTopology::TriangleList,
            },
            .fragment = &fragmentState,
        };

        pipeline = ctx.m_Device.CreateRenderPipeline(&renderPipelineDescriptor);
    }


    inline void RenderTest(Core::Context &ctx, wgpu::CommandEncoder encoder, wgpu::SurfaceTexture &surfaceTexture) {
        wgpu::RenderPassColorAttachment colorAttachment = {
            .view = surfaceTexture.texture.CreateView(),
            .loadOp = wgpu::LoadOp::Load,
            .storeOp = wgpu::StoreOp::Store,
        };

        wgpu::RenderPassDescriptor renderPassDescriptor{
            .label = "Test Render Pass",
            .colorAttachmentCount = 1,
            .colorAttachments = &colorAttachment,
        };

        wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDescriptor);
        renderPass.SetPipeline(pipeline);
        renderPass.SetVertexBuffer(0, vertexBuffer);
        renderPass.SetVertexBuffer(1, instanceBuffer);
        renderPass.Draw(vertices.size(), 2, 0, 0);
        renderPass.End();
    }
}
