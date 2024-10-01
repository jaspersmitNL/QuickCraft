#pragma once
#include <webgpu/webgpu_cpp.h>

class WebGPU;
class Shader;

class Pipeline {
public:
    WebGPU &m_WebGPU;
    Shader &m_Shader;
    const char* m_Label;

    wgpu::RenderPipeline m_Pipeline;

    Pipeline(WebGPU &webgpu, Shader &shader, const char* label);

    wgpu::RenderPipeline& CreatePipeline();

};