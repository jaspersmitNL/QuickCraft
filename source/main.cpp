#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/Context.hpp"
#include "game/Pipelines.hpp"
#include "core/Utils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"
#include "game/Camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    uint32_t blockID;
};


std::vector<Vertex> vertices2 = {


    //BACK
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left

    //FRONT
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}}, // b-left

    //LEFT
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}}, // b-right
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left

    //RIGHT
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}}, // t-left
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left

    //TOP
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left

    //BOTTOM
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left
    {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}}, // b-right
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f}}, // t-right
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}}, // t-left
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // b-left


};
std::vector<Vertex> vertices;

struct Uniforms {
    glm::mat4 uProjection;
    glm::mat4 uView;
    glm::mat4 uModel;
} uniforms;

Camera camera(45.0f, 0.1f, 200.0f);

Pipelines pipelines;
wgpu::Texture depthTexture;
wgpu::Buffer vertexBuffer;
wgpu::Buffer uniformBuffer;
wgpu::Texture texture;
wgpu::Sampler sampler;
wgpu::BindGroup uniformBindGroup;


unsigned char *LoadImage(const char *path) {
    int width, height, channels;
    unsigned char *pixels = stbi_load(path, &width, &height, &channels, 0);
    if (!pixels) {
        printf("Failed to load image: %s\n", path);
        exit(1);
    }
    if (channels == 3) {
        unsigned char *pixels4 = new unsigned char[width * height * 4];
        for (int i = 0; i < width * height; i++) {
            pixels4[i * 4] = pixels[i * 3];
            pixels4[i * 4 + 1] = pixels[i * 3 + 1];
            pixels4[i * 4 + 2] = pixels[i * 3 + 2];
            pixels4[i * 4 + 3] = 255;
        }
        printf("Converted to 4 channels\n");
        delete[] pixels;
        pixels = std::move(pixels4);
    }

    printf("Loaded texture %s width: %d, height: %d, channels: %d\n", path, width, height, channels);

    return pixels;
}

void LoadTextures(Core::Context &ctx) {
    wgpu::TextureDescriptor textureDescriptor{
        .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst |
                 wgpu::TextureUsage::RenderAttachment,
        .size = {128, 128, 2},
        .format = wgpu::TextureFormat::RGBA8Unorm,
    };

    texture = ctx.m_Device.CreateTexture(&textureDescriptor);


    //load one.png
    {
        wgpu::ImageCopyTexture destination;
        destination.texture = texture;
        destination.mipLevel = 0;
        destination.origin = {0, 0, 0};
        destination.aspect = wgpu::TextureAspect::All;

        wgpu::TextureDataLayout source;
        source.offset = 0;
        source.bytesPerRow = 128 * 4;
        source.rowsPerImage = 128;

        wgpu::Extent3D copySize = {128, 128, 1};
        unsigned char *pixels = LoadImage("../res/one.png");
        ctx.m_Queue.WriteTexture(&destination, pixels, 4 * 128 * 128, &source, &copySize);


        wgpu::SamplerDescriptor samplerDescriptor;
        samplerDescriptor.minFilter = wgpu::FilterMode::Linear;
        samplerDescriptor.magFilter = wgpu::FilterMode::Linear;
        sampler = ctx.m_Device.CreateSampler(&samplerDescriptor);
    }

    //load two.png
    {
        wgpu::ImageCopyTexture destination;
        destination.texture = texture;
        destination.mipLevel = 0;
        destination.origin = {0, 0, 1};
        destination.aspect = wgpu::TextureAspect::All;

        wgpu::TextureDataLayout source;
        source.offset = 0;
        source.bytesPerRow = 128 * 4;
        source.rowsPerImage = 128;

        wgpu::Extent3D copySize = {128, 128, 1};
        unsigned char *pixels = LoadImage("../res/two.png");
        ctx.m_Queue.WriteTexture(&destination, pixels, 4 * 128 * 128, &source, &copySize);


        wgpu::SamplerDescriptor samplerDescriptor;
        samplerDescriptor.minFilter = wgpu::FilterMode::Linear;
        samplerDescriptor.magFilter = wgpu::FilterMode::Linear;
        sampler = ctx.m_Device.CreateSampler(&samplerDescriptor);
    }

}


void Render(Core::Context &ctx) {
    camera.OnUpdate(1.0f / 120.0f);

    wgpu::SurfaceTexture surfaceTexture;
    ctx.m_Surface.GetCurrentTexture(&surfaceTexture);

    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = wgpu::Color{0.0f, 0.0f, 0.0f, 1.0f},
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

    uniforms.uProjection = camera.GetProjection();
    uniforms.uView = camera.GetView();

    float time = (float) glfwGetTime();
    uniforms.uModel = glm::mat4(1.0f);

    // uniforms.uModel = glm::rotate(glm::mat4(1.0f), time, glm::vec3(1.0f, 1.0f, 0.0f));
    ctx.m_Queue.WriteBuffer(uniformBuffer, 0, &uniforms, sizeof(Uniforms));


    wgpu::CommandEncoder encoder = ctx.m_Device.CreateCommandEncoder();


    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
    pass.SetPipeline(pipelines.m_ChunkPipeline);
    pass.SetBindGroup(0, uniformBindGroup);


    pass.SetVertexBuffer(0, vertexBuffer);
    pass.Draw(vertices.size(), 1, 0, 0);
    pass.End();


    // Start the Dear ImGui frame
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    bool demo = true;
    ImGui::ShowDemoWindow(&demo);


    ImGui::Render();


    wgpu::RenderPassColorAttachment imGuiColorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store,
    };
    wgpu::RenderPassDescriptor imGuiRenderPassDescriptor{
        .label = "ImGui Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &imGuiColorAttachment,
    };

    wgpu::RenderPassEncoder imGuiPass = encoder.BeginRenderPass(&imGuiRenderPassDescriptor);
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), imGuiPass.Get());
    imGuiPass.End();

    wgpu::CommandBuffer commands = encoder.Finish();
    ctx.m_Queue.Submit(1, &commands);
}

void SetupImGui(Core::Context &ctx) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOther(ctx.m_Window, true);
    ImGui::StyleColorsDark();

    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = ctx.m_Device.Get();
    init_info.NumFramesInFlight = 3;
    init_info.RenderTargetFormat = (WGPUTextureFormat) ctx.m_SurfaceTextureFormat;;
    init_info.DepthStencilFormat = WGPUTextureFormat_Undefined;
    ImGui_ImplWGPU_Init(&init_info);
}

void Start() {
    Core::Context ctx(1080, 720, "WebGPU");


    LoadTextures(ctx);


    for (int x = -8; x < 8; x++) {
        for (int y = -8; y < 8; y++)
            for (int z = -8; z < 8; z++) {

                uint32_t blockID = (x + y + z) % 2 == 0;


                auto verts = vertices2;
                for (auto &v: verts) {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                    glm::vec3 pos(model * glm::vec4(v.pos, 1.0f));

                    vertices.push_back({pos, v.uv, blockID});

                }
            }
    }


    vertexBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, vertices.data(),
                                              vertices.size() * sizeof(Vertex));


    uniformBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
                                               &uniforms, sizeof(Uniforms));

    pipelines.Initialize(ctx);


    depthTexture = ctx.m_Device.CreateTexture(ToPtr(wgpu::TextureDescriptor{
        .label = "Depth Texture",
        .usage = wgpu::TextureUsage::RenderAttachment,
        .dimension = wgpu::TextureDimension::e2D,
        .size = {ctx.m_Width, ctx.m_Height, 1},
        .format = wgpu::TextureFormat::Depth24Plus,
        .mipLevelCount = 1,
        .sampleCount = 1
    }));

    wgpu::BindGroupEntry bindGroupEntries[3] = {
        {
            .binding = 0,
            .buffer = uniformBuffer,
            .offset = 0,
            .size = sizeof(Uniforms),
        },
        {
            .binding = 1,
            .sampler = sampler,
        },
        {
            .binding = 2,
            .textureView = texture.CreateView()
        }
    };

    wgpu::BindGroupDescriptor bindGroupDescriptor{
        .label = "Uniform Bind Group",
        .layout = pipelines.m_ChunkPipeline.GetBindGroupLayout(0),
        .entryCount = 3,
        .entries = bindGroupEntries,
    };

    uniformBindGroup = ctx.m_Device.CreateBindGroup(&bindGroupDescriptor);


    camera.OnResize(ctx.m_Width, ctx.m_Height);
    camera.RecalculateProjection();
    camera.RecalculateView();


    SetupImGui(ctx);


    while (!glfwWindowShouldClose(Core::Context::m_Window)) {
        glfwPollEvents();
        int width, height;
        glfwGetFramebufferSize((GLFWwindow *) Core::Context::m_Window, &width, &height);

        if (width != ctx.m_Width || height != ctx.m_Height) {
            ctx.OnResize(width, height);

            depthTexture.Destroy();
            depthTexture = ctx.m_Device.CreateTexture(ToPtr(wgpu::TextureDescriptor{
                .label = "Depth Texture",
                .usage = wgpu::TextureUsage::RenderAttachment,
                .dimension = wgpu::TextureDimension::e2D,
                .size = {ctx.m_Width, ctx.m_Height, 1},
                .format = wgpu::TextureFormat::Depth24Plus,
                .mipLevelCount = 1,
                .sampleCount = 1
            }));

            camera.OnResize(width, height);
        }

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
