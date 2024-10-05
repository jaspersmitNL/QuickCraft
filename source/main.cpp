#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/Context.hpp"
#include "game/Pipelines.hpp"
#include "core/Utils.hpp"


struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
};

struct Uniforms {
    glm::mat4 uProjection;
};

Pipelines pipelines;
wgpu::Buffer vertexBuffer;
wgpu::Buffer uniformBuffer;
wgpu::BindGroup uniformBindGroup;


void Render(Core::Context &ctx) {
    wgpu::SurfaceTexture surfaceTexture;
    ctx.m_Surface.GetCurrentTexture(&surfaceTexture);

    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = wgpu::Color{0.0f, 0.0f, 0.0f, 1.0f},
    };

    wgpu::RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.colorAttachmentCount = 1;
    renderPassDescriptor.colorAttachments = &colorAttachment;


    wgpu::CommandEncoder encoder = ctx.m_Device.CreateCommandEncoder();
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
    pass.SetPipeline(pipelines.m_ChunkPipeline);
    pass.SetBindGroup(0, uniformBindGroup);


    pass.SetVertexBuffer(0, vertexBuffer);
    pass.Draw(6, 1, 0, 0);


    pass.End();
    wgpu::CommandBuffer commands = encoder.Finish();
    ctx.m_Queue.Submit(1, &commands);
}

void Start() {
    Core::Context ctx(1080, 720, "WebGPU");


    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}}, // bottom left
        {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}}, // bottom right
        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}}, // top right

        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}}, // top right
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}}, // top left
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}}, // bottom left

    };

    vertexBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, vertices.data(),
                                              vertices.size() * sizeof(Vertex));



    Uniforms uniforms{};
    uniforms.uProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f);

    uniformBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
                                               &uniforms, sizeof(Uniforms));

    pipelines.Initialize(ctx);

    wgpu::BindGroupEntry bindGroupEntries[1] = {
        {
            .binding = 0,
            .buffer = uniformBuffer,
            .offset = 0,
            .size = sizeof(Uniforms),
        },
    };

    wgpu::BindGroupDescriptor bindGroupDescriptor{
        .label = "Uniform Bind Group",
        .layout = pipelines.m_ChunkPipeline.GetBindGroupLayout(0),
        .entryCount = 1,
        .entries = bindGroupEntries,
    };

    uniformBindGroup = ctx.m_Device.CreateBindGroup(&bindGroupDescriptor);






    while (!glfwWindowShouldClose(Core::Context::m_Window)) {
        glfwPollEvents();

        Render(ctx);

        ctx.m_Surface.Present();
        ctx.m_Instance.ProcessEvents();
    }
}


int main() {
    std::cout << "Hello, World!" << std::endl;

    Start();

    return 0;
}
