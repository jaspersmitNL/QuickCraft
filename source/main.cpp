#include <iostream>

#include "core/shader.hpp"
#include "core/buffer.hpp"
#include "core/webgpu.hpp"
#include "core/pipeline.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Uniforms {
    glm::mat4 projection;
    glm::mat4 model;
};
struct Vertex {
    float position[2];
    float color[3];
};
Pipeline *renderPipeline;
Buffer<Vertex> *vertexBuffer;
wgpu::BindGroup bindGroup;




Buffer<Uniforms> *uniformBuffer;
Uniforms uniforms;

void Render(WebGPU &webgpu) {
    wgpu::SurfaceTexture surfaceTexture;
    webgpu.m_Surface.GetCurrentTexture(&surfaceTexture);


    wgpu::RenderPassColorAttachment renderPassColorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = wgpu::Color{0.1f, 0.2f, 0.3f, 1.0f},
    };


    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &renderPassColorAttachment,
    };


    wgpu::CommandEncoderDescriptor commandEncoderDescriptor{
        .label = "Command Encoder",
    };

    wgpu::CommandEncoder commandEncoder = webgpu.m_Device.CreateCommandEncoder(&commandEncoderDescriptor);
    wgpu::RenderPassEncoder passEncoder = commandEncoder.BeginRenderPass(&renderPassDescriptor);

    uniforms.projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f);
    uniforms.model = glm::mat4(1.0f);
    float scale = sin(glfwGetTime());
    uniforms.model = glm::scale(uniforms.model, glm::vec3(scale, scale, 1.0f));
    uniformBuffer->Write(uniforms);



    passEncoder.SetPipeline(renderPipeline->m_Pipeline);


    for (auto& vbc: renderPipeline->m_VertexConfigs) {
        passEncoder.SetVertexBuffer(0, vbc.buffer, vbc.offset, vbc.buffer.GetSize());
    }

    passEncoder.SetBindGroup(0, bindGroup);
    passEncoder.Draw(3);
    passEncoder.End();


    wgpu::CommandBuffer commandBuffer = commandEncoder.Finish();
    webgpu.m_Queue.Submit(1, &commandBuffer);
}

int main() {
    printf("Hello, World!\n");


    WebGPU webgpu(800, 600, "WebGPU Test");


    Shader shader("Simple Shader", "../res/shader.wgsl");
    renderPipeline = new Pipeline(webgpu, shader, "Render Pipeline");





    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    };


    // wgpu::BufferDescriptor vertexBufferDescriptor{
    //     .label = "Vertex Buffer",
    //     .usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst,
    //     .size = sizeof(Vertex) * vertices.size()
    // };
    // renderPipeline->m_VertexBuffer = webgpu.m_Device.CreateBuffer(&vertexBufferDescriptor);
    //
    // webgpu.m_Queue.WriteBuffer(renderPipeline->m_VertexBuffer, 0, vertices.data(), sizeof(Vertex) * vertices.size());
    //


    Buffer<Vertex> vertexBuffer(webgpu, "Vertex Buffer", vertices, wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst);


    renderPipeline->AddVertexBuffer(vertexBuffer);
    renderPipeline->AddVertexAttribute(wgpu::VertexFormat::Float32x2, 0);
    renderPipeline->AddVertexAttribute(wgpu::VertexFormat::Float32x3, 1);
    renderPipeline->CreatePipeline();


    uniformBuffer = new Buffer<Uniforms>(webgpu, "Uniforms", {uniforms},
                                         wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst);


    wgpu::BindGroupEntry bindGroupEntries[] = {
        {
            .binding = 0,
            .buffer = uniformBuffer->m_Buffer,
            .offset = 0,
            .size = sizeof(Uniforms),
        },
    };

    wgpu::BindGroupDescriptor bindGroupDescriptor = {
        .label = "Uniform Bind Group",
        .layout = renderPipeline->m_Pipeline.GetBindGroupLayout(0),
        .entryCount = 1,
        .entries = bindGroupEntries,
    };

    bindGroup = webgpu.m_Device.CreateBindGroup(&bindGroupDescriptor);


    webgpu.SetVisible(true);

    while (!glfwWindowShouldClose(webgpu.m_Window)) {
        glfwPollEvents();


        Render(webgpu);

        webgpu.m_Surface.Present();
        webgpu.m_Instance.ProcessEvents();
    }


    return 0;
}
