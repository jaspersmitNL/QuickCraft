#pragma once
#include "core/Context.hpp"


namespace Core {
    class RenderPipelineBuilder {
    public:
        RenderPipelineBuilder(Core::Context &context) : m_Context(&context) {
        }

        RenderPipelineBuilder &SetShaderModule(wgpu::ShaderModule shaderModule);

        RenderPipelineBuilder &SetVertexEntryPoint(const std::string &entryPoint);

        RenderPipelineBuilder &SetFragmentEntryPoint(const std::string &entryPoint);

        RenderPipelineBuilder &AddVertexBufferLayout(const std::vector<wgpu::VertexAttribute>& attributes, uint32_t stride = 0,
                                                     wgpu::VertexStepMode stepMode = wgpu::VertexStepMode::Vertex);

        RenderPipelineBuilder &EnableDepthStencil(bool enabled = true);

        wgpu::RenderPipeline Build(const std::string &label = "Render Pipeline");

    private:
        Core::Context *m_Context;
        wgpu::ShaderModule m_ShaderModule;
        std::string m_VertexEntryPoint = "vs_main", m_FragmentEntryPoint = "fs_main";
        std::vector<wgpu::VertexBufferLayout> m_VertexBufferLayouts;
        bool m_DepthStencilEnabled = false;
    };

}
