#include <glm/glm.hpp>
#include "Pipelines.hpp"
#include "Vertex.hpp"
#include "core/Utils.hpp"


Pipelines::~Pipelines() {
    m_ChunkPipeline = nullptr;
    m_ChunkBindGroupLayout = nullptr;
    printf("Pipelines destroyed\n");
}

void Pipelines::Initialize(Core::Context &ctx) {

    OnResize(ctx);

    wgpu::ShaderModule chunkShaderModule = Core::LoadShaderFromFile(ctx.m_Device, "../res/test.wgsl", "ChunkShader");


    static std::vector<wgpu::VertexAttribute> vertexAttributes{
        {wgpu::VertexFormat::Float32x3, offsetof(Vertex, pos), 0}, // pos
        {wgpu::VertexFormat::Float32x2, offsetof(Vertex, uv), 1}, // uv
        {wgpu::VertexFormat::Uint32, offsetof(Vertex, blockID), 2}, // blockID
    };


    m_ChunkPipeline = ctx.m_Device.CreateRenderPipeline(ToPtr(wgpu::RenderPipelineDescriptor{
        .vertex = wgpu::VertexState{
            .module = chunkShaderModule,
            .entryPoint = "vs_main",
            .bufferCount = 1,
            .buffers = ToPtr(wgpu::VertexBufferLayout{
                .arrayStride = sizeof(Vertex),
                .attributeCount = vertexAttributes.size(),
                .attributes = vertexAttributes.data(),
            }),
        },
        .primitive = {
            .topology = wgpu::PrimitiveTopology::TriangleList,
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

void Pipelines::OnResize(Core::Context &ctx) {
    if(m_DepthTexture) {
        m_DepthTexture.Destroy();
    }
    wgpu::TextureDescriptor depthTextureDescriptor{
        .label = "Depth Texture",
          .usage = wgpu::TextureUsage::RenderAttachment,
          .dimension = wgpu::TextureDimension::e2D,
          .size = {ctx.m_Width, ctx.m_Height, 1},
          .format = wgpu::TextureFormat::Depth24Plus,
          .mipLevelCount = 1,
          .sampleCount = 1
    };

    m_DepthTexture = ctx.m_Device.CreateTexture(&depthTextureDescriptor);
}
