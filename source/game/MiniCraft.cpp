#include "MiniCraft.hpp"
#include "core/builder/RenderPipelineBuilder.hpp"

#include <stdio.h>
#include <webgpu/webgpu_cpp.h>
#include <GLFW/glfw3.h>

#include "world/World.hpp"
#include "world/render/WorldRenderer.hpp"


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
        .clearValue = wgpu::Color{0.0f, 0.0f, 0.0f, 1.0f},
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

    m_WorldRenderer->Render(encoder, surfaceTexture);


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
}

void MiniCraft::Run() {
    printf("Running MiniCraft\n");

    Init();

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



MiniCraft::~MiniCraft() {
    printf("Destroying MiniCraft\n");
}
