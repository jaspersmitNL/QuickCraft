#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>
#include "Context.hpp"
#include "Utils.hpp"


namespace Core {
    void Context::SetupAdapter() {
        wgpu::Future future = m_Instance.RequestAdapter(
            ToPtr(wgpu::RequestAdapterOptions{
                .compatibleSurface = m_Surface,
                .powerPreference = wgpu::PowerPreference::HighPerformance,
            }),
            wgpu::CallbackMode::WaitAnyOnly,
            [](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter,
               const char *message, wgpu::Adapter *userdata) {
                if (status != wgpu::RequestAdapterStatus::Success) {
                    fprintf(stderr, "Failed to request adapter: %d\n",
                            status);
                    if (message) {
                        fprintf(stderr, "Message: %s\n", message);
                    }
                    return;
                }
                *userdata = std::move(adapter);
                printf("WebGPU Adapter initialized\n");
            },
            &m_Adapter);
        WAIT_FOR_FUTURE(m_Instance, future);
    }

    void Context::SetupDevice() {



        wgpu::DeviceDescriptor deviceDescriptor{};
        deviceDescriptor.label = "Default Device";
        deviceDescriptor.deviceLostCallbackInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
        deviceDescriptor.deviceLostCallbackInfo.callback = [](const WGPUDevice *device, WGPUDeviceLostReason reason,
                                                              char const *message, void *) {
            std::cout << "Info: WGPU: Device " << device << " lost: reason " << reason;
            if (message) std::cout << " (" << message << ")";
            std::cout << std::endl;
        };
        deviceDescriptor.uncapturedErrorCallbackInfo.callback = [](WGPUErrorType error, char const *message, void *) {
            fprintf(stderr, "WebGPU ERROR: %d\n", error);
            if (message) {
                fprintf(stderr, "Message: %s\n", message);
            }
        };

        wgpu::RequiredLimits requiredLimits{};
        requiredLimits.limits.maxBufferSize = 1099511627776; //

        deviceDescriptor.requiredLimits = &requiredLimits;


        wgpu::Future future = m_Adapter.RequestDevice(
            &deviceDescriptor,
            wgpu::CallbackMode::WaitAnyOnly,
            [](wgpu::RequestDeviceStatus status, wgpu::Device device,
               const char *message, wgpu::Device *userdata) {
                if (status != wgpu::RequestDeviceStatus::Success) {
                    fprintf(stderr, "Failed to request device: %d\n",
                            status);
                    if (message) {
                        fprintf(stderr, "Message: %s\n", message);
                    }
                    return;
                }
                *userdata = std::move(device);
                printf("WebGPU Device initialized\n");
            },
            &m_Device);

        WAIT_FOR_FUTURE(m_Instance, future);
    }

    void Context::SetupRenderer() {
        m_Surface = wgpu::glfw::CreateSurfaceForWindow(m_Instance, m_Window);


        wgpu::AdapterInfo adapterInfo;
        m_Adapter.GetInfo(&adapterInfo);
        std::cout << "Adapter name: " << adapterInfo.device << std::endl;

        m_Queue = m_Device.GetQueue();
        m_Surface.GetCapabilities(m_Adapter, &m_SurfaceCapabilities);
        m_SurfaceTextureFormat = m_SurfaceCapabilities.formats[0];


        m_Surface.Configure(ToPtr(wgpu::SurfaceConfiguration{

            .device = m_Device,
            .format = m_SurfaceTextureFormat,
            .width = m_Width,
            .height = m_Height,
            .presentMode = wgpu::PresentMode::Immediate
        }));
    }


    Context::Context(int width, int height, const char *title) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        m_Width = width;
        m_Height = height;
        m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        m_Instance = wgpu::CreateInstance(ToPtr(wgpu::InstanceDescriptor{.features{.timedWaitAnyEnable{true}}}));

        glfwSetWindowUserPointer(m_Window, this);


        SetupAdapter();
        SetupDevice();
        SetupRenderer();

        // center the window
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(m_Window, (mode->width - m_Width) / 2, (mode->height - m_Height) / 2);


        glfwShowWindow(m_Window);
    }

    void Context::OnResize(uint32_t width, uint32_t height) {
        this->m_Width = width;
        this->m_Height = height;

        // Recreate the surface
        m_Surface.Configure(ToPtr(wgpu::SurfaceConfiguration{
            .device = m_Device,
            .format = m_SurfaceTextureFormat,
            .width = m_Width,
            .height = m_Height,
            .presentMode = wgpu::PresentMode::Immediate
        }));

        // Recreate the swap chain
        // Recreate the render pass

    }
}
