#include "core/pipeline.hpp"
#include "core/webgpu.hpp"
#include "core/shader.hpp"

Pipeline::Pipeline(WebGPU &webgpu, Shader &shader, const char *label)
    : m_WebGPU(webgpu), m_Shader(shader), m_Label(label)
{
}

wgpu::RenderPipeline& Pipeline::CreatePipeline() {
    wgpu::ShaderModule shaderModule = m_Shader.Transfer(m_WebGPU);

    wgpu::ColorTargetState colorTargetState{
        .format = m_WebGPU.m_TextureFormat
    };
    wgpu::FragmentState fragmentState{
        .module = shaderModule,
        .targetCount = 1,
        .targets = &colorTargetState
    };

    wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
        .label = this->m_Label,
        .vertex = {.module = shaderModule},
        .fragment = &fragmentState,
    };


    this->m_Pipeline = this->m_WebGPU.m_Device.CreateRenderPipeline(&renderPipelineDescriptor);


    return this->m_Pipeline;
}
