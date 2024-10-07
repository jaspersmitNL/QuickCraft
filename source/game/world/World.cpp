#include "World.hpp"
#include "core/Utils.hpp"
#include "game/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

int count = 0;

void World::Initialize(Core::Context &ctx) {
    std::vector<BlockFaceVert> vertices = {
        {glm::vec3(-0.5f, -0.5f, 0.5f)}, // bottom left
        {glm::vec3(0.5f, -0.5f, 0.5f)}, // bottom right
        {glm::vec3(0.5f, 0.5f, 0.5f)}, // top right
        {glm::vec3(0.5f, 0.5f, 0.5f)}, // top right
        {glm::vec3(-0.5f, 0.5f, 0.5f)}, // top left
        {glm::vec3(-0.5f, -0.5f, 0.5f)}, // bottom left
    };


    std::vector<BlockFace> AllFaces = {
        {{0.0f, 0.0f, 0.0f}, 0}, //front
        {{0.0f, 0.0f, 0.0f}, 1}, // back
        {{0.0f, 0.0f, 0.0f}, 2}, //left
        {{0.0f, 0.0f, 0.0f}, 3}, //right
        {{0.0f, 0.0f, 0.0f}, 4}, //top
        {{0.0f, 0.0f, 0.0f}, 5}, //bottom
    };

    std::vector<BlockFace> faces;


#define SIZE 8

    for (int x = -SIZE; x < SIZE; x++) {
        for (int z = -SIZE; z < SIZE; z++) {
            for(int y = 0; y < SIZE; y++) {
                for(auto face : AllFaces) {
                    face.center = glm::vec3(x, y, z);
                    faces.push_back(face);
                }
            }

        }
    }


    count = faces.size();

    m_VertexBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, vertices.data(),
                                                sizeof(BlockFaceVert) * vertices.size());

    m_InstanceBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, faces.data(),
                                                  sizeof(BlockFace) * faces.size());

    m_UniformBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
                                                 &m_Uniforms, sizeof(WorldUniforms));


    m_ShaderModule = Core::LoadShaderFromFile(ctx.m_Device, "../res/world.wgsl", "World");


    wgpu::ColorTargetState colorTargetState{
        .format = ctx.m_SurfaceTextureFormat,
    };

    wgpu::FragmentState fragmentState{
        .module = m_ShaderModule,
        .entryPoint = "fs_main",
        .targetCount = 1,
        .targets = &colorTargetState,
    };

    wgpu::VertexAttribute vertexAttributes[1] = {
        {
            .format = wgpu::VertexFormat::Float32x3,
            .offset = 0,
            .shaderLocation = 0,
        }
    };
    wgpu::VertexAttribute instanceAttributes[2] = {
        {
            .format = wgpu::VertexFormat::Float32x3,
            .offset = 0,
            .shaderLocation = 1,
        },
        {
            .format = wgpu::VertexFormat::Uint32,
            .offset = sizeof(glm::vec3),
            .shaderLocation = 2,
        }
    };
    wgpu::VertexBufferLayout bufferLayouts[2] = {
        {
            .arrayStride = sizeof(BlockFaceVert),
            .stepMode = wgpu::VertexStepMode::Vertex,
            .attributeCount = 1,
            .attributes = vertexAttributes,
        },
        {
            .arrayStride = sizeof(BlockFace),
            .stepMode = wgpu::VertexStepMode::Instance,
            .attributeCount = 2,
            .attributes = instanceAttributes,
        }
    };

    wgpu::VertexState vertexState{
        .module = m_ShaderModule,
        .entryPoint = "vs_main",
        .bufferCount = 2,
        .buffers = bufferLayouts,
    };

    wgpu::DepthStencilState depthStencilState{
        .format = wgpu::TextureFormat::Depth24Plus,
        .depthWriteEnabled = {true},
        .depthCompare = wgpu::CompareFunction::Less,
    };

    wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
        .label = "World Render Pipeline",
        .vertex = vertexState,
        .primitive = {
            .topology = wgpu::PrimitiveTopology::TriangleList,
        },
        .depthStencil = &depthStencilState,
        .fragment = &fragmentState,
    };

    m_Pipeline = ctx.m_Device.CreateRenderPipeline(&renderPipelineDescriptor);

    wgpu::BindGroupEntry bindGroupEntries[1] = {
        {
            .binding = 0,
            .buffer = m_UniformBuffer,
        },
    };
    wgpu::BindGroupDescriptor bindGroupDescriptor{
        .layout = m_Pipeline.GetBindGroupLayout(0),
        .entryCount = 1,
        .entries = bindGroupEntries,
    };

    m_BindGroup = ctx.m_Device.CreateBindGroup(&bindGroupDescriptor);
}

void World::Render(Core::Context &ctx, wgpu::CommandEncoder encoder, wgpu::SurfaceTexture &surfaceTexture,
                   wgpu::Texture &depthTexture, Camera &camera) {
    m_Uniforms.projection = camera.GetProjection();
    m_Uniforms.view = camera.GetView();

    ctx.m_Device.GetQueue().WriteBuffer(m_UniformBuffer, 0, &m_Uniforms, sizeof(WorldUniforms));


    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {0.0f, 0.0f, 0.0f, 1.0f},
    };

    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = depthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Load,
        .depthStoreOp = wgpu::StoreOp::Store
    };

    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "World Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachment,
        .depthStencilAttachment = &renderPassDepthStencilAttachment,
    };

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
    pass.SetPipeline(m_Pipeline);
    pass.SetBindGroup(0, m_BindGroup);
    pass.SetVertexBuffer(0, m_VertexBuffer);
    pass.SetVertexBuffer(1, m_InstanceBuffer);
    pass.Draw(6, count, 0, 0);
    pass.End();
}
