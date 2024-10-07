#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>


#include "core/Context.hpp"
#include "core/Utils.hpp"
#include "game/Pipelines.hpp"
#include "game/Vertex.hpp"
#include "game/Camera.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"
#include "FastNoiseLite.hpp"
#include "Test.hpp"
#include "game/world/Chunk.hpp"
#include "game/world/World.hpp"


Camera camera(45.0f, 0.1f, 500.0f);

Pipelines pipelines;

wgpu::Texture texture;
wgpu::Sampler sampler;
std::vector<Chunk> chunks = {};

World world;


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
        .size = {128, 128, 4},
        .format = wgpu::TextureFormat::RGBA8Unorm,
    };

    texture = ctx.m_Device.CreateTexture(&textureDescriptor);


    //load one.png
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
        destination.origin = {0, 0, 2};
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


    wgpu::CommandEncoder encoder = ctx.m_Device.CreateCommandEncoder();


    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = wgpu::Color{0.1f, 0.2f, 0.3f, 1.0f},
    };

    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = pipelines.m_DepthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Clear,
        .depthStoreOp = wgpu::StoreOp::Store,
        .depthClearValue = 1.0f
    };

    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "Clear Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachment,
        .depthStencilAttachment = &renderPassDepthStencilAttachment,
    };

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
    pass.End();

    // for (auto &chunk: chunks) {
    //     chunk.Render(ctx, pipelines, encoder, surfaceTexture, camera);
    // }




    // Start the Dear ImGui frame
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    world.Render(ctx, encoder, surfaceTexture, pipelines.m_DepthTexture, camera);




    ImGui::Begin("Camera");
    auto pos = camera.GetPosition();
    if(ImGui::InputFloat3("Pos", &pos.x)) {
        camera.SetPosition(pos);
    }
    ImGui::End();


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

    pipelines.Initialize(ctx);

    world.Initialize(ctx);


    // for(int x = -4; x < 4; x++) {
    //     for(int z = -4; z < 4; z++) {
    //         chunks.emplace_back(glm::vec3(x, 0, z));
    //     }
    // }
    // printf("c: %d\n", chunks.size());
    //
    //
    // for (auto &chunk: chunks) {
    //     chunk.GenerateChunk();
    //     chunk.BuildMesh(ctx, pipelines, sampler, texture);
    // }


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
            pipelines.OnResize(ctx);

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
