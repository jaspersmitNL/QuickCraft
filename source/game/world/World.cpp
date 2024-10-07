#include "World.hpp"
#include "core/Utils.hpp"
#include "game/Camera.hpp"
#include "FastNoiseLite.hpp"

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



    int seed = 1337;

    FastNoiseLite terrain;
    terrain.SetSeed(seed);
    terrain.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    terrain.SetFrequency(0.001f);

    FastNoiseLite forest;
    forest.SetSeed(seed);
    forest.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    forest.SetFractalType(FastNoiseLite::FractalType_Ridged);


    FastNoiseLite mountain;
    mountain.SetSeed(seed);
    mountain.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    mountain.SetFractalType(FastNoiseLite::FractalType_Ridged);
    mountain.SetFrequency(0.005f);


    FastNoiseLite river;
    river.SetSeed(seed);
    river.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    river.SetFractalType(FastNoiseLite::FractalType_Ridged);
    river.SetFrequency(0.001f);



    auto getHeight = [&](float x, float z) {
        int height;

        float noise = abs(terrain.GetNoise(x, z) + 0.5f);
        if (noise < 0.05f)
        {
            height = (int)abs(8 * (river.GetNoise(x, z) + 0.8f)) + 30;
        }

        else if (noise < 1.2f)
        {
            height = (int)abs(10 * (forest.GetNoise(x, z) + 0.8f)) + 30;
        }

        else
        {
            height = (int)abs(30 * (mountain.GetNoise(x, z) + 0.8f)) + 30;
        }

        return height;
    };

#define SIZE 90

    for (int x = -SIZE; x < SIZE; x++) {
        for (int z = -SIZE; z < SIZE; z++) {
            int height = getHeight(x, z);
            for(int y = 0; y < height; y++) {



                for(auto face : AllFaces) {
                    //checkerboard using x y z
                    face.blockID = (x + y + z) % 2 == 0 ? 1 : 2;
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
    wgpu::VertexAttribute instanceAttributes[3] = {
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
            .attributeCount = 3,
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
