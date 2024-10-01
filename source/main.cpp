#include <iostream>
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


    const char shaderCode[] = R"(
   struct Uniforms {
    uTime: f32
   };
@group(0) @binding(0) var<uniform> uniforms: Uniforms;
    @vertex fn vertexMain(@builtin(vertex_index) i : u32) ->
      @builtin(position) vec4f {
        const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
        //scale the position by the time

        return vec4f(pos[i] * sin(uniforms.uTime), 0, 1);
    }
    @fragment fn fragmentMain() -> @location(0) vec4f {
        return vec4f(1, 0, 0, 1);
    }
)";
    wgpu::ShaderModuleWGSLDescriptor shaderModuleWGSLDescriptor{};
    shaderModuleWGSLDescriptor.code = shaderCode;
    wgpu::ShaderModuleDescriptor shaderModuleDescriptor{
        .nextInChain = &shaderModuleWGSLDescriptor,
    };

    wgpu::ShaderModule shaderModule = webgpu.m_Device.CreateShaderModule(&shaderModuleDescriptor);

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
