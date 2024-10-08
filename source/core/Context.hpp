#pragma once
#include <webgpu/webgpu_glfw.h>
#include <webgpu/webgpu_cpp.h>

namespace Core {
    class Context {
    public:
        uint32_t m_Width;
        uint32_t m_Height;
        inline static GLFWwindow* m_Window = nullptr;
        wgpu::Instance m_Instance;
        wgpu::Surface m_Surface;
        wgpu::Adapter m_Adapter;
        wgpu::Device m_Device;
        wgpu::Queue m_Queue;

        wgpu::SurfaceCapabilities m_SurfaceCapabilities;
        wgpu::TextureFormat m_SurfaceTextureFormat;


        wgpu::Texture m_DepthTexture;


        Context(int width, int height, const char* title);
        ~Context();

        void OnResize(uint32_t width, uint32_t height);

    private:
        void SetupAdapter();
        void SetupDevice();
        void SetupRenderer();
    };
}
