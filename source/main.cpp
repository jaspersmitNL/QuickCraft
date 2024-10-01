#include <iostream>

#include "core/shader.hpp"
#include "core/buffer.hpp"
#include "core/webgpu.hpp"
#include "core/pipeline.hpp"



Pipeline* renderPipeline;
wgpu::BindGroup bindGroup;


struct Uniforms {
    float time;
};

Buffer<Uniforms>* uniformBuffer;


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

    passEncoder.SetPipeline(renderPipeline->m_Pipeline);

    uniforms.time = (float)glfwGetTime();
    uniformBuffer->Write(uniforms);




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

    renderPipeline->CreatePipeline();





    uniformBuffer = new Buffer<Uniforms>(webgpu, "Uniforms", {uniforms}, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst);



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



    while (!glfwWindowShouldClose(webgpu.m_Window)) {
        glfwPollEvents();


        Render(webgpu);

        webgpu.m_Surface.Present();
        webgpu.m_Instance.ProcessEvents();
    }


    return 0;
}
