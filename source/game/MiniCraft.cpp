#include "MiniCraft.hpp"
#include "world/Chunk.hpp"
#include "core/builder/RenderPipelineBuilder.hpp"
#include "core/Utils.hpp"

#include <stdio.h>
#include <webgpu/webgpu_cpp.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



wgpu::ShaderModule shader;
wgpu::RenderPipeline pipeline;
wgpu::Buffer vertexBuffer;
wgpu::Buffer instanceBuffer;
wgpu::Buffer uniformBuffer;
wgpu::BindGroup bindGroup;

Ref<Chunk> chunk;


Uniforms uniforms{};


int faceCount = 0;


void BuildChunkMesh(Ref<Core::Context> ctx) {
    std::vector<BlockFace> faces;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                uint32_t blockID = chunk->GetBlock(x, y, z);
                if (blockID == 0) {
                    continue;
                }

                for (auto face: Chunk::blockFaces) {
                    face.center = glm::vec3(x, y, z);
                    face.blockID = blockID;
                    faces.push_back(face);
                }
            }
        }
    }
    printf("Got %d faces\n", faces.size());




    faceCount = faces.size();

    vertexBuffer = Core::CreateBufferFromData(ctx->m_Device, wgpu::BufferUsage::Vertex, Chunk::blockVertices.data(),
                                            sizeof(BlockVertex) * Chunk::blockVertices.size());

    instanceBuffer = Core::CreateBufferFromData(ctx->m_Device, wgpu::BufferUsage::Vertex, faces.data(),
                                                  sizeof(BlockFace) * faces.size());

    uniformBuffer = Core::CreateBufferFromData(ctx->m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
                                                 &uniformBuffer, sizeof(Uniforms));

    shader = Core::LoadShaderFromFile(ctx->m_Device, "../res/world.wgsl", "World");




    Core::RenderPipelineBuilder builder(*ctx);

    pipeline = builder
        .SetShaderModule(shader)
        .SetVertexEntryPoint("vs_main")
        .SetFragmentEntryPoint("fs_main")
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



    wgpu::BindGroupEntry bindGroupEntries[1] = {
        {
            .binding = 0,
            .buffer = uniformBuffer,
        },
    };
    wgpu::BindGroupDescriptor bindGroupDescriptor{
        .layout = pipeline.GetBindGroupLayout(0),
        .entryCount = 1,
        .entries = bindGroupEntries,
    };

    bindGroup = ctx->m_Device.CreateBindGroup(&bindGroupDescriptor);

}


MiniCraft::MiniCraft() {
    m_RenderContext = CreateRef<Core::Context>(1080, 720, "MiniCraft");


    m_Camera = CreateRef<Camera>(45.0f, 0.1f, 200.0f);
    chunk = CreateRef<Chunk>(glm::vec3(0, 1, 0));


    m_Camera->OnResize(m_RenderContext->m_Width, m_RenderContext->m_Height);
    m_Camera->RecalculateProjection();
    m_Camera->RecalculateView();

    chunk->Generate();

    BuildChunkMesh(m_RenderContext);
}

void MiniCraft::OnRender() {
    auto &ctx = m_RenderContext;

    m_Camera->OnUpdate(1.0f / 120.0f);


    uniforms.projection = m_Camera->GetProjection();
    uniforms.view = m_Camera->GetView();

    glm::vec3 chunkOffset = {
        chunk->m_Position.x * CHUNK_SIZE,
        chunk->m_Position.y * CHUNK_SIZE,
        chunk->m_Position.z * CHUNK_SIZE
    };

    uniforms.model = glm::translate(glm::mat4(1.0f), chunkOffset);

    ctx->m_Device.GetQueue().WriteBuffer(uniformBuffer, 0, &uniforms, sizeof(Uniforms));






    wgpu::SurfaceTexture surfaceTexture;
    ctx->m_Surface.GetCurrentTexture(&surfaceTexture);

    wgpu::CommandEncoder encoder = ctx->m_Device.CreateCommandEncoder();

    wgpu::RenderPassColorAttachment clearColorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = wgpu::Color{0.0f, 0.0f, 0.0f, 1.0f},
    };

    wgpu::RenderPassDepthStencilAttachment clearDepthAttachment{
        .view = ctx->m_DepthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Clear,
        .depthStoreOp = wgpu::StoreOp::Store,
        .depthClearValue = 1.0f
    };

    wgpu::RenderPassDescriptor clearPassDescriptor{
        .label = "Clear pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &clearColorAttachment,
        .depthStencilAttachment = &clearDepthAttachment
    };

    wgpu::RenderPassEncoder clearPass = encoder.BeginRenderPass(&clearPassDescriptor);
    clearPass.End();



    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
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

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
    pass.SetPipeline(pipeline);
    pass.SetBindGroup(0, bindGroup);
    pass.SetVertexBuffer(0, vertexBuffer);
    pass.SetVertexBuffer(1, instanceBuffer);
    pass.Draw(6, faceCount, 0, 0);
    pass.End();

    wgpu::CommandBuffer commands = encoder.Finish();
    ctx->m_Queue.Submit(1, &commands);
}


void MiniCraft::Run() {
    printf("Running MiniCraft\n");

    while (!glfwWindowShouldClose(m_RenderContext->m_Window)) {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(m_RenderContext->m_Window, &width, &height);

        width = std::max(width, 1);
        height = std::max(height, 1);

        if (width != m_RenderContext->m_Width || height != m_RenderContext->m_Height) {
            m_RenderContext->OnResize(width, height);
        }


        OnRender();


        m_RenderContext->m_Surface.Present();
        m_RenderContext->m_Instance.ProcessEvents();
    }
}


MiniCraft::~MiniCraft() {
    printf("Destroying MiniCraft\n");
}
