#pragma once
#include "core/Context.hpp"
#include "core/Ref.hpp"


namespace Core {
    class RenderPipelineBuilder {
    public:
        RenderPipelineBuilder(Ref<Context> ctx) : m_Context(ctx) {
        }

        RenderPipelineBuilder &SetShaderModule(wgpu::ShaderModule shaderModule);

        RenderPipelineBuilder &SetVertexEntryPoint(const std::string &entryPoint);

        RenderPipelineBuilder &SetFragmentEntryPoint(const std::string &entryPoint);

        RenderPipelineBuilder &AddVertexBufferLayout(const std::vector<wgpu::VertexAttribute>& attributes, uint32_t stride = 0,
                                                     wgpu::VertexStepMode stepMode = wgpu::VertexStepMode::Vertex);

        RenderPipelineBuilder &EnableDepthStencil(bool enabled = true);

        wgpu::RenderPipeline Build(const std::string &label = "Render Pipeline");

    private:
        Ref<Core::Context> m_Context;
        wgpu::ShaderModule m_ShaderModule;
        std::string m_VertexEntryPoint = "vs_main", m_FragmentEntryPoint = "fs_main";
        std::vector<wgpu::VertexBufferLayout> m_VertexBufferLayouts;
        bool m_DepthStencilEnabled = false;
    };

}
