#include <glm/glm.hpp>
#include "Pipelines.hpp"
#include "core/Utils.hpp"


void Pipelines::Initialize(Core::Context &ctx) {
    wgpu::ShaderModule chunkShaderModule = Core::LoadShaderFromFile(ctx.m_Device, "../res/test.wgsl", "ChunkShader");

    struct Vertex {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    static std::vector<wgpu::VertexAttribute> vertexAttributes{
        {wgpu::VertexFormat::Float32x3, offsetof(Vertex, pos), 0}, // pos
        {wgpu::VertexFormat::Float32x2, offsetof(Vertex, uv), 1}, // uv
    };


    m_ChunkPipeline = ctx.m_Device.CreateRenderPipeline(ToPtr(wgpu::RenderPipelineDescriptor{
        .vertex = wgpu::VertexState{
            .module = chunkShaderModule,
            .entryPoint = "vs_main",
            .bufferCount = 1,
            .buffers = ToPtr(wgpu::VertexBufferLayout{
                .arrayStride = sizeof(Vertex),
                .attributeCount = 2,
                .attributes = vertexAttributes.data(),
            }),
        },
        .depthStencil = ToPtr(wgpu::DepthStencilState{
            .format = wgpu::TextureFormat::Depth24Plus,
            .depthWriteEnabled = {true},
            .depthCompare = wgpu::CompareFunction::Less,
        }),
        .fragment = ToPtr(wgpu::FragmentState{
            .module = chunkShaderModule,
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = ToPtr(wgpu::ColorTargetState{
                .format = ctx.m_SurfaceTextureFormat
            })
        }),

    }));
}
