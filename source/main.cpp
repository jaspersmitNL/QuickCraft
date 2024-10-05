#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/Context.hpp"
#include "core/Utils.hpp"
#include "game/Camera.hpp"


struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

struct Uniforms {
    glm::mat4 uProj;
    glm::mat4 uView;
    glm::mat4 uModel;
};

Camera camera(45.0f, 0.1f, 200.0f);

wgpu::Texture depthTexture;
wgpu::RenderPipeline renderPipeline;
wgpu::Buffer vertexBuffer;
wgpu::Buffer uniformBuffer;
wgpu::BindGroup bindGroup;
std::vector<Vertex> vertices;



glm::vec3 RED = {1.0f, 0.0f, 0.0f};
glm::vec3 GREEN = {0.0f, 1.0f, 0.0f};
glm::vec3 BLUE = {0.0f, 0.0f, 1.0f};
glm::vec3 YELLOW = {1.0f, 1.0f, 0.0f};
glm::vec3 CYAN = {0.0f, 1.0f, 1.0f};
glm::vec3 MAGENTA = {1.0f, 0.0f, 1.0f};


enum EnumFace : uint8_t {
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5
};

std::vector<Vertex> GetVerts(EnumFace face, glm::vec3 color) {
    std::vector<Vertex> verts;

    switch (face) {
        case FRONT: {
            verts = {
                {{-0.5f, -0.5f, 0.5f}, color}, // bottom left
                {{0.5f, -0.5f, 0.5f}, color}, // bottom right
                {{0.5f, 0.5f, 0.5f}, color}, // top right
                {{0.5f, 0.5f, 0.5f}, color}, // top right
                {{-0.5f, 0.5f, 0.5f}, color}, // top left
                {{-0.5f, -0.5f, 0.5f}, color}, // bottom left
            };
            break;
        }
        case BACK: {
            verts = {
                {{-0.5f, -0.5f, -0.5f}, color}, // bottom left
                {{0.5f, -0.5f, -0.5f}, color}, // bottom right
                {{0.5f, 0.5f, -0.5f}, color}, // top right
                {{0.5f, 0.5f, -0.5f}, color}, // top right
                {{-0.5f, 0.5f, -0.5f}, color}, // top left
                {{-0.5f, -0.5f, -0.5f}, color}, // bottom left
            };
            break;
        }
        case LEFT: {
            verts = {
                {{-0.5f, 0.5f, 0.5f}, color}, // top right
                {{-0.5f, 0.5f, -0.5f}, color}, // top left
                {{-0.5f, -0.5f, -0.5f}, color}, // bottom left
                {{-0.5f, -0.5f, -0.5f}, color}, // bottom left
                {{-0.5f, -0.5f, 0.5f}, color}, // bottom right
                {{-0.5f, 0.5f, 0.5f}, color}, // top right
            };
            break;
        }
        case RIGHT: {
            verts = {
                {{0.5f, 0.5f, 0.5f}, color}, // top left
                {{0.5f, 0.5f, -0.5f}, color}, // top right
                {{0.5f, -0.5f, -0.5f}, color}, // bottom right
                {{0.5f, -0.5f, -0.5f}, color}, // bottom right
                {{0.5f, -0.5f, 0.5f}, color}, // bottom left
                {{0.5f, 0.5f, 0.5f}, color}, // top left
            };
            break;
        }
        case TOP: {
            verts = {
                {{-0.5f, 0.5f, -0.5f}, color}, // top right
                {{0.5f, 0.5f, -0.5f}, color}, // top left
                {{0.5f, 0.5f, 0.5f}, color}, // bottom left
                {{0.5f, 0.5f, 0.5f}, color}, // bottom left
                {{-0.5f, 0.5f, 0.5f}, color}, // bottom right
                {{-0.5f, 0.5f, -0.5f}, color}, // top right
            };
            break;
        }
        case BOTTOM: {
            verts = {
                {{-0.5f, -0.5f, -0.5f}, color}, // top right
                {{0.5f, -0.5f, -0.5f}, color}, // top left
                {{0.5f, -0.5f, 0.5f}, color}, // bottom left
                {{0.5f, -0.5f, 0.5f}, color}, // bottom left
                {{-0.5f, -0.5f, 0.5f}, color}, // bottom right
                {{-0.5f, -0.5f, -0.5f}, color}, // top right
            };
            break;
        }
    }

    return verts;
}

void SetupRenderPipeline(Core::Context &ctx) {
    wgpu::TextureDescriptor depthTextureDescriptor{
        .label = "Depth Texture",
        .usage = wgpu::TextureUsage::RenderAttachment,
        .dimension = wgpu::TextureDimension::e2D,
        .size = {ctx.m_Width, ctx.m_Height, 1},
        .format = wgpu::TextureFormat::Depth24Plus,
        .mipLevelCount = 1,
        .sampleCount = 1,
    };

    depthTexture = ctx.m_Device.CreateTexture(&depthTextureDescriptor);


    std::string shaderCode = R"(
    struct VertexInput {
        @location(0) pos: vec3<f32>,
        @location(1) color: vec3<f32>,
    }
    struct VertexOutput {
        @builtin(position) pos: vec4<f32>,
        @location(0) color: vec4<f32>
    }

    struct Uniforms {
        uProj: mat4x4<f32>,
        uView: mat4x4<f32>,
        uModel: mat4x4<f32>,
    }

    @group(0) @binding(0) var<uniform> uniforms: Uniforms;

    @vertex fn vertexMain(in: VertexInput) -> VertexOutput {
        var out: VertexOutput;
        out.pos = uniforms.uProj * uniforms.uView * uniforms.uModel * vec4<f32>(in.pos, 1.0);
        out.color = vec4<f32>(in.color, 1.0);
        return out;
    }
    @fragment fn fragmentMain(in: VertexOutput) -> @location(0) vec4f {
        return in.color;
    }
)";

    wgpu::ShaderModule shaderModule = Core::LoadShader(ctx.m_Device, shaderCode, "SimpleShader");


    wgpu::ColorTargetState colorTargetState{
        .format = ctx.m_SurfaceTextureFormat,
    };

    wgpu::FragmentState fragmentState{
        .module = shaderModule,
        .entryPoint = "fragmentMain",
        .targetCount = 1,
        .targets = &colorTargetState,
    };


    wgpu::VertexAttribute attributes[2] = {
        {
            .format = wgpu::VertexFormat::Float32x3,
            .offset = offsetof(Vertex, pos),
            .shaderLocation = 0,
        },
        {
            .format = wgpu::VertexFormat::Float32x3,
            .offset = offsetof(Vertex, color),
            .shaderLocation = 1,
        },
    };

    wgpu::VertexBufferLayout vertexBufferLayout{
        .arrayStride = sizeof(Vertex),
        .attributeCount = 2,
        .attributes = attributes,
    };


    wgpu::VertexState vertexState{
        .module = shaderModule,
        .entryPoint = "vertexMain",
        .bufferCount = 1,
        .buffers = &vertexBufferLayout,
    };


    wgpu::DepthStencilState depthStencilState{
        .format = wgpu::TextureFormat::Depth24Plus,
        .depthWriteEnabled = {true},
        .depthCompare = wgpu::CompareFunction::Less,
    };

    wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
        .label = "Render Pipeline",
        .vertex = vertexState,
        .primitive = {
            .topology = wgpu::PrimitiveTopology::LineList,
            .cullMode = wgpu::CullMode::None
        },
        .depthStencil = &depthStencilState,
        .fragment = &fragmentState,
    };


    renderPipeline = ctx.m_Device.CreateRenderPipeline(&renderPipelineDescriptor);
}

void Setup(Core::Context &ctx) {


    SetupRenderPipeline(ctx);


    auto addFace = [&](EnumFace face, glm::vec3 color, glm::vec3 pos = {0.0f, 0.0f, 0.0f}) {
        auto v = GetVerts(face, color);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        for (auto &vertex : v) {
            vertex.pos = glm::vec3(model * glm::vec4(vertex.pos, 1.0f));
        }
        vertices.insert(vertices.end(), v.begin(), v.end());
    };

    std::vector<glm::vec3> blocks = {

    };

    // for(int x = -10; x < 10; x++) {
    //     for(int z = -10; z < 10; z++) {
    //         blocks.emplace_back(x, 0, z);
    //     }
    // }

    // gen a chunk of 16x16x16 blocks
    for(int x = 0; x < 16; x++) {
        for(int y = 0; y < 16; y++) {
            for(int z = 0; z < 16; z++) {
                blocks.emplace_back(x, y, z);
            }
        }
    }

    for(auto& block : blocks)
    {
        addFace(FRONT, RED, block);
        addFace(BACK, BLUE, block);
        addFace(LEFT, GREEN, block);
        addFace(RIGHT, YELLOW, block);
        addFace(TOP, CYAN, block);
        addFace(BOTTOM, MAGENTA, block);
    }



    vertexBuffer = ctx.m_Device.CreateBuffer(Core::ToPtr(wgpu::BufferDescriptor{
        .usage = wgpu::BufferUsage::Vertex,
        .size = sizeof(Vertex) * vertices.size(),
        .mappedAtCreation = true,
    }));

    void *data = vertexBuffer.GetMappedRange();
    memcpy(data, vertices.data(), sizeof(Vertex) * vertices.size());
    vertexBuffer.Unmap();


    wgpu::BufferDescriptor uniformBufferDescriptor{
        .label = "Uniform Buffer",
        .usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
        .size = sizeof(Uniforms),
    };

    uniformBuffer = ctx.m_Device.CreateBuffer(&uniformBufferDescriptor);


    wgpu::BindGroupEntry bindGroupEntries[1] = {
        {
            .binding = 0,
            .buffer = uniformBuffer,
            .offset = 0,
            .size = sizeof(Uniforms),
        },
    };

    wgpu::BindGroupDescriptor bindGroupDescriptor{
        .label = "Uniform Bind Group",
        .layout = renderPipeline.GetBindGroupLayout(0),
        .entryCount = 1,
        .entries = bindGroupEntries,
    };

    bindGroup = ctx.m_Device.CreateBindGroup(&bindGroupDescriptor);
}

void Render(Core::Context &ctx) {
    wgpu::SurfaceTexture surfaceTexture;
    ctx.m_Surface.GetCurrentTexture(&surfaceTexture);

    wgpu::CommandEncoder commandEncoder = ctx.m_Device.CreateCommandEncoder(Core::ToPtr(wgpu::CommandEncoderDescriptor{
        .label = "Command Encoder",
    }));

    wgpu::RenderPassColorAttachment colorAttachment = {
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = wgpu::Color{0.1f, 0.2f, 0.3f, 1.0f},
    };

    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = depthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Clear,
        .depthStoreOp = wgpu::StoreOp::Store,
        .depthClearValue = 1.0f,
    };

    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachment,
        .depthStencilAttachment = &renderPassDepthStencilAttachment,
    };

    wgpu::RenderPassEncoder renderPass = commandEncoder.BeginRenderPass(&renderPassDescriptor);


    camera.OnUpdate(1.0f / 60.0f);


    Uniforms uniforms{};

    uniforms.uProj = camera.GetProjection();
    uniforms.uView = camera.GetView();



    uniforms.uModel = glm::mat4(1.0f);
    auto time = static_cast<float>(glfwGetTime());
    // uniforms.uModel = glm::rotate(uniforms.uModel, time, glm::vec3(1.0f, 1.0f, 0.0f));

    ctx.m_Queue.WriteBuffer(uniformBuffer, 0, &uniforms, sizeof(Uniforms));
    renderPass.SetPipeline(renderPipeline);
    renderPass.SetBindGroup(0, bindGroup);
    renderPass.SetVertexBuffer(0, vertexBuffer);


    renderPass.Draw(vertices.size(), 1, 0, 0);
    renderPass.End();
    wgpu::CommandBuffer commandBuffer = commandEncoder.Finish();
    ctx.m_Queue.Submit(1, &commandBuffer);
}

void Start() {
    Core::Context ctx(1080, 720, "WebGPU");

    Setup(ctx);

    glfwShowWindow(Core::Context::m_Window);

    camera.OnResize(ctx.m_Width, ctx.m_Height);
    camera.RecalculateProjection();
    camera.RecalculateView();

    while (!glfwWindowShouldClose(Core::Context::m_Window)) {
        glfwPollEvents();

        Render(ctx);

        ctx.m_Surface.Present();
        ctx.m_Instance.ProcessEvents();
    }
}


int main() {
    std::cout << "Hello, World!" << std::endl;

    Start();

    return 0;
}
