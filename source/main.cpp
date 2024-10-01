#include <iostream>

#include "core/shader.hpp"
#include "core/webgpu.hpp"

wgpu::RenderPipeline renderPipeline;

wgpu::Buffer uniformBuffer;
wgpu::BindGroup bindGroup;

struct Uniforms {
    float time;
};

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

    passEncoder.SetPipeline(renderPipeline);

    uniforms.time = (float)glfwGetTime();
    webgpu.m_Queue.WriteBuffer(uniformBuffer, 0, &uniforms, sizeof(uniforms));



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

    wgpu::ShaderModule shaderModule = shader.Transfer(webgpu);

    wgpu::ColorTargetState colorTargetState{
        .format = webgpu.m_TextureFormat
    };
    wgpu::FragmentState fragmentState{
        .module = shaderModule,
        .targetCount = 1,
        .targets = &colorTargetState
    };

    wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
        .label = "Simple Pipeline",
        .vertex = {.module = shaderModule},
        .fragment = &fragmentState,
    };

    renderPipeline = webgpu.m_Device.CreateRenderPipeline(&renderPipelineDescriptor);



    wgpu::BufferDescriptor uniformBufferDescriptor = {
        .label = "Uniform Buffer",
        .usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
        .size = sizeof(Uniforms),
    };

    uniformBuffer = webgpu.m_Device.CreateBuffer(&uniformBufferDescriptor);
    webgpu.m_Queue.WriteBuffer(uniformBuffer, 0, &uniforms, sizeof(uniforms));
    wgpu::BindGroupEntry bindGroupEntries[] = {
        {
            .binding = 0,
            .buffer = uniformBuffer,
            .offset = 0,
            .size = sizeof(Uniforms),
        },
    };

    wgpu::BindGroupDescriptor bindGroupDescriptor = {
        .label = "Uniform Bind Group",
        .layout = renderPipeline.GetBindGroupLayout(0),
        .entryCount = 1,
        .entries = bindGroupEntries,
    };
    bindGroup = webgpu.m_Device.CreateBindGroup(&bindGroupDescriptor);



    while (!glfwWindowShouldClose(webgpu.m_Window)) {
        glfwPollEvents();


        Render(webgpu);

        webgpu.m_Surface.Present();
        webgpu.m_Instance.ProcessEvents();
    }


    return 0;
}
