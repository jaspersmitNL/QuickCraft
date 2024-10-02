#pragma once
#include <webgpu/webgpu_cpp.h>
#include "core/buffer.hpp"

class WebGPU;
class Shader;

struct VBC {
    wgpu::Buffer buffer;
    wgpu::VertexStepMode stepMode;
    uint64_t offset;
    std::vector<wgpu::VertexAttribute> attributes;
};


class Pipeline {
private:

    std::vector<wgpu::VertexBufferLayout> m_VertexBufferLayouts;

public:
    WebGPU &m_WebGPU;
    Shader &m_Shader;
    const char *m_Label;

    wgpu::RenderPipeline m_Pipeline;
    wgpu::Texture m_DepthTexture;
    std::vector<VBC> m_VertexConfigs;
    Pipeline(WebGPU &webgpu, Shader &shader, const char *label);


    void AddVertexBuffer(wgpu::Buffer buffer, wgpu::VertexStepMode stepMode = wgpu::VertexStepMode::Vertex,
                         uint64_t offset = 0) {
        m_VertexConfigs.push_back({buffer, stepMode, offset, {}});
    }

    template<typename T>
    void AddVertexBuffer(Buffer<T> buffer, wgpu::VertexStepMode stepMode = wgpu::VertexStepMode::Vertex,
                         uint64_t offset = 0) {
        AddVertexBuffer(buffer.m_Buffer, stepMode, offset);
    }

    void AddVertexAttribute(wgpu::VertexFormat format, uint32_t location, uint64_t offset = ~0, uint64_t index = ~0);

    wgpu::RenderPipeline &CreatePipeline();
};
