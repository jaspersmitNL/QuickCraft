#include <iostream>

#include "core/shader.hpp"
#include "core/buffer.hpp"
#include "core/webgpu.hpp"
#include "core/pipeline.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/camera.hpp"

struct Uniforms {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

Pipeline *renderPipeline;
Buffer<Vertex> *vertexBuffer;
wgpu::BindGroup bindGroup;
Buffer<Uniforms> *uniformBuffer;
Uniforms uniforms;

Camera camera(45.0f, 0.1f, 200.0f);

uint32_t vertexCount;

void Render(WebGPU &webgpu, double deltaTime) {
    wgpu::SurfaceTexture surfaceTexture;
    webgpu.m_Surface.GetCurrentTexture(&surfaceTexture);


    wgpu::RenderPassColorAttachment renderPassColorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = wgpu::Color{0.1f, 0.2f, 0.3f, 1.0f},
    };


    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = renderPipeline->m_DepthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Clear,
        .depthStoreOp = wgpu::StoreOp::Store,
        .depthClearValue = 1.0f,

    };

    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &renderPassColorAttachment,
        .depthStencilAttachment = &renderPassDepthStencilAttachment,
    };


    wgpu::CommandEncoderDescriptor commandEncoderDescriptor{
        .label = "Command Encoder",
    };

    wgpu::CommandEncoder commandEncoder = webgpu.m_Device.CreateCommandEncoder(&commandEncoderDescriptor);
    wgpu::RenderPassEncoder renderPass = commandEncoder.BeginRenderPass(&renderPassDescriptor);


    renderPass.SetPipeline(renderPipeline->m_Pipeline);
    renderPass.SetBindGroup(0, bindGroup);

    for (auto &vbc: renderPipeline->m_VertexConfigs) {
        renderPass.SetVertexBuffer(0, vbc.buffer, vbc.offset, vbc.buffer.GetSize());
    }


    uniforms.view = camera.GetView();
    uniforms.projection = camera.GetProjection();


    uniforms.model = glm::mat4(1.0f);
    uniforms.model = glm::scale(uniforms.model, glm::vec3(0.5f, 0.5f, 0.5f));
    uniforms.model = glm::translate(uniforms.model, glm::vec3(-5.0f, 0.0f, 0.0f));
    uniformBuffer->Write(uniforms);
    renderPass.Draw(vertexCount, 1, 0, 0);


    renderPass.End();
    wgpu::CommandBuffer commandBuffer = commandEncoder.Finish();
    webgpu.m_Queue.Submit(1, &commandBuffer);
}

int main() {
    printf("Hello, World!\n");


    WebGPU webgpu(1280, 720, "QuickCraft");

    glfwSwapInterval(1); // Enable vsync


    Shader shader("Simple Shader", "../res/shader.wgsl");
    renderPipeline = new Pipeline(webgpu, shader, "Render Pipeline");

    glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);

    std::vector<Vertex> cube_verts = {

        // FRONT
        {{-0.5f, -0.5f, 0.5f}, RED}, // b-left
        {{0.5f, -0.5f, 0.5f}, RED}, // b-right
        {{0.5f, 0.5f, 0.5f}, RED}, // t-right

        {{0.5f, 0.5f, 0.5f}, RED}, // t-right
        {{-0.5f, 0.5f, 0.5f}, RED}, // t-left
        {{-0.5f, -0.5f, 0.5f}, RED}, // b-left
        // END FRONT

        // BACK
        {{-0.5f, -0.5f, -0.5f}, GREEN}, // b-left
        {{0.5f, -0.5f, -0.5f}, GREEN}, // b-right
        {{0.5f, 0.5f, -0.5f}, GREEN}, // t-right

        {{0.5f, 0.5f, -0.5f}, GREEN}, // t-right
        {{-0.5f, 0.5f, -0.5f}, GREEN}, // t-left
        {{-0.5f, -0.5f, -0.5f}, GREEN}, // b-left
        // END BACK

        // LEFT
        {{-0.5f, -0.5f, -0.5f}, BLUE}, // b-left
        {{-0.5f, -0.5f, 0.5f}, BLUE}, // b-right
        {{-0.5f, 0.5f, 0.5f}, BLUE}, // t-right

        {{-0.5f, 0.5f, 0.5f}, BLUE}, // t-right
        {{-0.5f, 0.5f, -0.5f}, BLUE}, // t-left
        {{-0.5f, -0.5f, -0.5f}, BLUE}, // b-left
        // END LEFT

        // RIGHT
        {{0.5f, -0.5f, -0.5f}, YELLOW}, // b-left
        {{0.5f, -0.5f, 0.5f}, YELLOW}, // b-right
        {{0.5f, 0.5f, 0.5f}, YELLOW}, // t-right

        {{0.5f, 0.5f, 0.5f}, YELLOW}, // t-right
        {{0.5f, 0.5f, -0.5f}, YELLOW}, // t-left
        {{0.5f, -0.5f, -0.5f}, YELLOW}, // b-left

        // END RIGHT


    };

    vertexCount = cube_verts.size();

    Buffer<Vertex> vertexBuffer(webgpu, "Vertex Buffer", cube_verts,
                                wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst);


    renderPipeline->AddVertexBuffer(vertexBuffer);
    renderPipeline->AddVertexAttribute(wgpu::VertexFormat::Float32x3, 0);
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


    double lastTime = glfwGetTime();

    camera.OnResize(webgpu.m_Width, webgpu.m_Height);
    camera.RecalculateProjection();
    camera.RecalculateView();

    while (!glfwWindowShouldClose(WebGPU::m_Window)) {
        glfwPollEvents();

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        camera.OnResize(webgpu.m_Width, webgpu.m_Height);
        camera.OnUpdate(deltaTime);

        Render(webgpu, deltaTime);

        webgpu.m_Surface.Present();
        webgpu.m_Instance.ProcessEvents();

        lastTime = currentTime;
    }


    return 0;
}
