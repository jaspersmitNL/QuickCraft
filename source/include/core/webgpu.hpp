#pragma once
#include <string>
#include <webgpu/webgpu_cpp.h>
#include "GLFW/glfw3.h"




class WebGPU {
public:
    uint32_t m_Width, m_Height;

    GLFWwindow* m_Window;
    wgpu::Surface m_Surface;
    wgpu::SurfaceCapabilities m_SurfaceCapabilities{};
    wgpu::TextureFormat m_TextureFormat;

    wgpu::Instance m_Instance;
    wgpu::Adapter m_Adapter;
    wgpu::Device m_Device;
    wgpu::Queue m_Queue;


    WebGPU(uint32_t width, uint32_t height, const std::string& title);
    ~WebGPU();

    void SetVisible(bool show);
};
