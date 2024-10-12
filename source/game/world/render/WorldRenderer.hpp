#pragma once
#include "../World.hpp"
#include <webgpu/webgpu_cpp.h>

class WorldRenderer {


public:
    WorldRenderer(Ref<World> world): m_World(world) {}

    void Init();

    void Render(wgpu::CommandEncoder& commandEncoder, wgpu::SurfaceTexture& surfaceTexture);

    wgpu::RenderPipeline m_Pipeline;
private:
    Ref<World> m_World;
    wgpu::Buffer m_VertexBuffer;
    wgpu::ShaderModule m_Shader;

};