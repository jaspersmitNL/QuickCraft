#include "RenderPipelineBuilder.hpp"

namespace Core {
    RenderPipelineBuilder &RenderPipelineBuilder::SetShaderModule(wgpu::ShaderModule shaderModule) {
        this->m_ShaderModule = shaderModule;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::SetVertexEntryPoint(const std::string &entryPoint) {
        this->m_VertexEntryPoint = entryPoint;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::SetFragmentEntryPoint(const std::string &entryPoint) {
        this->m_FragmentEntryPoint = entryPoint;
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::AddVertexBufferLayout(
        const std::vector<wgpu::VertexAttribute> &attributes,
        uint32_t stride, wgpu::VertexStepMode stepMode) {
        wgpu::VertexBufferLayout layout{};
        layout.arrayStride = stride;
        layout.stepMode = stepMode;
        layout.attributeCount = attributes.size();
        layout.attributes = attributes.data();

        m_VertexBufferLayouts.push_back(layout);
        return *this;
    }

    RenderPipelineBuilder &RenderPipelineBuilder::EnableDepthStencil(bool enabled) {
        m_DepthStencilEnabled = enabled;
        return *this;
    }

    wgpu::RenderPipeline RenderPipelineBuilder::Build(const std::string &label) {
        wgpu::VertexState vertexState{};
        vertexState.module = m_ShaderModule;
        vertexState.entryPoint = m_VertexEntryPoint.c_str();
        vertexState.bufferCount = m_VertexBufferLayouts.size();
        vertexState.buffers = m_VertexBufferLayouts.data();


        wgpu::ColorTargetState colorTargetState{};
        colorTargetState.format = m_Context->m_SurfaceTextureFormat;


        wgpu::FragmentState fragmentState{};
        fragmentState.module = m_ShaderModule;
        fragmentState.entryPoint = m_FragmentEntryPoint.c_str();
        fragmentState.targetCount = 1;
        fragmentState.targets = &colorTargetState;


        wgpu::DepthStencilState depthStencilState{};
        depthStencilState.format = wgpu::TextureFormat::Depth24Plus;
        depthStencilState.depthWriteEnabled = true;
        depthStencilState.depthCompare = wgpu::CompareFunction::Less;


        wgpu::RenderPipelineDescriptor descriptor{};
        descriptor.label = label.c_str();
        descriptor.vertex = vertexState;
        descriptor.primitive.topology = wgpu::PrimitiveTopology::TriangleList;

        descriptor.layout = nullptr;

        if (m_DepthStencilEnabled) {
            descriptor.depthStencil = &depthStencilState;
        }

        descriptor.fragment = &fragmentState;


        return m_Context->m_Device.CreateRenderPipeline(&descriptor);
    }
}
