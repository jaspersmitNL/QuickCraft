#pragma once
#include <webgpu/webgpu_cpp.h>
#include "core/Context.hpp"


class Pipelines {
public:

    // Depth Textures
    wgpu::Texture m_DepthTexture;

    // Bind Group Layouts
    wgpu::BindGroupLayout m_ChunkBindGroupLayout;

    //Pipelines
    wgpu::RenderPipeline m_ChunkPipeline;


    ~Pipelines();

    void Initialize(Core::Context& ctx);
    void OnResize(Core::Context& ctx);


};

