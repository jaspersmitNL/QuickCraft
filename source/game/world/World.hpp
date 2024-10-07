
#pragma once
#include <glm/glm.hpp>
#include "core/Context.hpp"

class Camera;

struct BlockFaceVert {
    glm::vec3 pos;
};

struct BlockFace {
    glm::vec3 center;
    uint32_t orientation;
    uint32_t blockID;
};

struct WorldUniforms {
    glm::mat4 projection;
    glm::mat4 view;
};

class World {
public:

    void Initialize(Core::Context& context);

    void Render(Core::Context &ctx, wgpu::CommandEncoder encoder, wgpu::SurfaceTexture &surfaceTexture, wgpu::Texture& depthTexture, Camera& camera);


private:
    wgpu::Buffer m_VertexBuffer;
    wgpu::Buffer m_InstanceBuffer;
    wgpu::Buffer m_UniformBuffer;
    WorldUniforms m_Uniforms{};
    wgpu::ShaderModule m_ShaderModule;
    wgpu::RenderPipeline m_Pipeline;
    wgpu::BindGroup m_BindGroup;


};



