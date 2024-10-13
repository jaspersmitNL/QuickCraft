#pragma once
#include "../World.hpp"
#include <webgpu/webgpu_cpp.h>


struct TextureObject {
    uint32_t idx;
    std::string path;
};


class WorldRenderer {

public:
    WorldRenderer(Ref<World> world): m_World(world) {}

    void LoadTextures(std::vector<TextureObject> textures);
    void Init();

    void Render(wgpu::CommandEncoder& commandEncoder, wgpu::SurfaceTexture& surfaceTexture);

    wgpu::RenderPipeline m_Pipeline;
    wgpu::Texture m_Texture;
    wgpu::Sampler m_Sampler;
    Uniforms m_Uniforms{};
private:
    Ref<World> m_World;
    wgpu::Buffer m_VertexBuffer;
    wgpu::Buffer m_UniformBuffer;
    wgpu::ShaderModule m_Shader;
    wgpu::BindGroup m_BindGroup;



};