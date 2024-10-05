#pragma once
#include <webgpu/webgpu_cpp.h>
#include "core/Context.hpp"



class Pipelines {
public:

    // Bind Group Layouts
    wgpu::BindGroupLayout m_ChunkBindGroupLayout;

    //Pipelines
    wgpu::RenderPipeline m_ChunkPipeline;

    void Initialize(Core::Context& ctx);
};

