#include "MiniCraft.hpp"
#include "core/builder/RenderPipelineBuilder.hpp"
#include "world/World.hpp"
#include "world/render/WorldRenderer.hpp"
#include "utils/Raycast.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>

#include <stdio.h>
#include <webgpu/webgpu_cpp.h>
#include <GLFW/glfw3.h>


wgpu::ShaderModule shader;





MiniCraft::MiniCraft() {
    m_RenderContext = CreateRef<Core::Context>(1080, 720, "MiniCraft");
    m_Camera = CreateRef<Camera>(45.0f, 0.1f, 800.0f);
    m_World = CreateRef<World>();
    m_WorldRenderer = CreateRef<WorldRenderer>(m_World);





}

void MiniCraft::OnRender() {
    auto &ctx = m_RenderContext;

    m_Camera->OnUpdate(1.0f / 120.0f);


    wgpu::SurfaceTexture surfaceTexture;
    ctx->m_Surface.GetCurrentTexture(&surfaceTexture);



    wgpu::CommandEncoder encoder = ctx->m_Device.CreateCommandEncoder();

    wgpu::RenderPassColorAttachment clearColorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {0.5f, 0.7f, 1.0f, 1.0f}
    };

    wgpu::RenderPassDepthStencilAttachment clearDepthAttachment{
        .view = ctx->m_DepthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Clear,
        .depthStoreOp = wgpu::StoreOp::Store,
        .depthClearValue = 1.0f
    };

    wgpu::RenderPassDescriptor clearPassDescriptor{
        .label = "Clear pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &clearColorAttachment,
        .depthStencilAttachment = &clearDepthAttachment
    };

    wgpu::RenderPassEncoder clearPass = encoder.BeginRenderPass(&clearPassDescriptor);
    clearPass.End();


    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    m_WorldRenderer->Render(encoder, surfaceTexture);


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
    ctx->m_Queue.Submit(1, &commands);
}



void MiniCraft::Init() {
    m_Camera->OnResize(m_RenderContext->m_Width, m_RenderContext->m_Height);
    m_Camera->RecalculateProjection();
    m_Camera->RecalculateView();

    m_WorldRenderer->Init();
    m_World->Init();

    m_World->Generate();
    m_World->BuildFullMesh();


    glfwSetKeyCallback(m_RenderContext->m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (action != GLFW_PRESS)
            return;

        if (key == GLFW_KEY_F1) {
            Get()->Test(true);
        }
        if (key == GLFW_KEY_F2) {
         Get()->Test(false);
     }
    });
}

void MiniCraft::Run() {
    printf("Running MiniCraft\n");

    Init();
    InitImGui();

    while (!glfwWindowShouldClose(m_RenderContext->m_Window)) {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(m_RenderContext->m_Window, &width, &height);

        width = std::max(width, 1);
        height = std::max(height, 1);

        if (width != m_RenderContext->m_Width || height != m_RenderContext->m_Height) {
            m_RenderContext->OnResize(width, height);
        }

        OnRender();


        m_RenderContext->m_Surface.Present();
        m_RenderContext->m_Instance.ProcessEvents();
    }
}




void MiniCraft::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOther(m_RenderContext->m_Window, true);
    ImGui::StyleColorsDark();

    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = m_RenderContext->m_Device.Get();
    init_info.NumFramesInFlight = 3;
    init_info.RenderTargetFormat = (WGPUTextureFormat) m_RenderContext->m_SurfaceTextureFormat;
    init_info.DepthStencilFormat = WGPUTextureFormat_Undefined;
    ImGui_ImplWGPU_Init(&init_info);
}

void MiniCraft::Test(bool place) {
    auto hit = Raycast::Cast(m_Camera->GetPosition(), m_Camera->GetDirection(), 5);

    if (hit.chunk) {
        printf("We hit a block at position: %f, %f, %f\n", hit.blockPos.x, hit.blockPos.y, hit.blockPos.z);
        hit.chunk->SetBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z, place ? 7 : 0);
        hit.chunk->BuildMesh();
    }


}
