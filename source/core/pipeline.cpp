#include "core/pipeline.hpp"
#include "core/webgpu.hpp"
#include "core/shader.hpp"

static constexpr uint64_t vertex_format_sizes[32] = {
    0,  // Undefined
    2,  // Uint8x2
    4,  // Uint8x4
    2,  // Sint8x2
    4,  // Sint8x4
    2,  // Unorm8x2
    4,  // Unorm8x4
    2,  // Snorm8x2
    4,  // Snorm8x4
    4,  // Uint16x2
    8,  // Uint16x4
    4,  // Sint16x2
    8,  // Sint16x4
    4,  // Unorm16x2
    8,  // Unorm16x4
    4,  // Snorm16x2
    8,  // Snorm16x4
    4,  // Float16x2
    8,  // Float16x4
    4,  // Float32
    8,  // Float32x2
    12, // Float32x3
    16, // Float32x4
    4,  // Uint32
    8,  // Uint32x2
    12, // Uint32x3
    16, // Uint32x4
    4,  // Sint32
    8,  // Sint32x2
    12, // Sint32x3
    16, // Sint32x4
    4,  // Unorm10_10_10_2
};

constexpr uint64_t vertex_format_size(const wgpu::VertexFormat& format) {
    return vertex_format_sizes[static_cast<size_t>(format)];
}

uint64_t vertex_attributes_stride(const std::vector<wgpu::VertexAttribute>& vertexAttributes) {
    uint64_t totalStride = 0;
    for (const auto& va : vertexAttributes) {
        totalStride += vertex_format_size(va.format);
    }
    return totalStride;
}


Pipeline::Pipeline(WebGPU &webgpu, Shader &shader, const char *label)
    : m_WebGPU(webgpu), m_Shader(shader), m_Label(label)
{
}



void Pipeline::AddVertexAttribute(wgpu::VertexFormat format, uint32_t location, uint64_t offset, uint64_t index) {
    uint64_t bi = index == ~0 ? m_VertexConfigs.size() - 1 : index; // Get the last buffer index
    offset = offset == ~0 ? vertex_attributes_stride(m_VertexConfigs.at(bi).attributes) : offset;

    this->m_VertexConfigs.at(bi).attributes.push_back({
        .format = format,
        .offset = offset,
        .shaderLocation = location,
    });

}

wgpu::RenderPipeline& Pipeline::CreatePipeline() {


    for (int i = 0; i < m_VertexConfigs.size(); ++i) {
        m_VertexBufferLayouts.push_back({
            .arrayStride = vertex_attributes_stride(m_VertexConfigs[i].attributes),
            .stepMode = m_VertexConfigs[i].stepMode,
            .attributeCount = m_VertexConfigs[i].attributes.size(),
            .attributes = m_VertexConfigs[i].attributes.data(),
        });
    }



    wgpu::ShaderModule shaderModule = m_Shader.Transfer(m_WebGPU);

    wgpu::VertexState vertexState{
        .module = shaderModule,
        .bufferCount = m_VertexBufferLayouts.size(),
        .buffers = m_VertexBufferLayouts.data(),
    };

    wgpu::ColorTargetState colorTargetState {
        .format = m_WebGPU.m_TextureFormat,
    };

    wgpu::FragmentState fragmentState{
        .module = shaderModule,
        .targetCount = 1,
        .targets = &colorTargetState,
    };

    wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
        .label = this->m_Label,
        .vertex = vertexState,
        .fragment = &fragmentState,
    };

    this->m_Pipeline = this->m_WebGPU.m_Device.CreateRenderPipeline(&renderPipelineDescriptor);


    return this->m_Pipeline;

    // struct Vertex {
    //     float position[2];
    //     float color[3];
    // };
    //
    // Vertex vertices[3] = {
    //     {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    //     {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    //     {{0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    // };
    //
    // wgpu::BufferDescriptor vertexBufferDescriptor{
    //     .label = "Vertex Buffer",
    //     .usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst,
    //     .size = sizeof(vertices)
    // };
    //
    // this->m_VertexBuffer = this->m_WebGPU.m_Device.CreateBuffer(&vertexBufferDescriptor);
    //
    //
    //
    // this->m_VertexBuffer.Unmap();
    //
    //
    // wgpu::VertexAttribute attributes[] = {
    //     {
    //         .format = wgpu::VertexFormat::Float32x2,
    //         .offset = offsetof(Vertex, position),
    //         .shaderLocation = 0,
    //     },
    //     {
    //         .format = wgpu::VertexFormat::Float32x3,
    //         .offset = offsetof(Vertex, color),
    //         .shaderLocation = 1,
    //     },
    // };
    // wgpu::VertexBufferLayout vertexBufferLayout = {
    //     .arrayStride = sizeof(Vertex),
    //     .attributeCount = 2,
    //     .attributes = attributes,
    // };
    //
    // wgpu::ShaderModule shaderModule = m_Shader.Transfer(m_WebGPU);
    //
    // wgpu::ColorTargetState colorTargetState{
    //     .format = m_WebGPU.m_TextureFormat
    // };
    // wgpu::FragmentState fragmentState{
    //     .module = shaderModule,
    //     .targetCount = 1,
    //     .targets = &colorTargetState
    // };
    //
    // wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
    //     .label = this->m_Label,
    //     .vertex = {
    //         .module = shaderModule,
    //         .bufferCount = 1,
    //         .buffers = &vertexBufferLayout,
    //     },
    //     .fragment = &fragmentState,
    // };
    //
    //
    // this->m_Pipeline = this->m_WebGPU.m_Device.CreateRenderPipeline(&renderPipelineDescriptor);


    // return this->m_Pipeline;
}
