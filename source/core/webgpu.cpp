#include "core/webgpu.hpp"
#include <webgpu/webgpu_glfw.h>
#include <iostream>

template<class T> T* Temp(T&& v) { return &v; }

#define WAIT_FOR_FUTURE(future) m_Instance.WaitAny(future, UINT64_MAX)


WebGPU::WebGPU(uint32_t width, uint32_t height, const std::string &title) {
    this->m_Width = width;
    this->m_Height = height;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(this->m_Width, this->m_Height, title.c_str(), nullptr, nullptr);
    this->m_Surface = wgpu::glfw::CreateSurfaceForWindow(m_Instance, m_Window);


    wgpu::InstanceDescriptor instanceDescriptor{.features{.timedWaitAnyEnable{true}}};
    this->m_Instance = wgpu::CreateInstance(&instanceDescriptor);

    wgpu::RequestAdapterOptions requestAdapterOptions = {
        .compatibleSurface = this->m_Surface,
        .powerPreference = wgpu::PowerPreference::HighPerformance,
    };

    wgpu::Future future = this->m_Instance.RequestAdapter(
        &requestAdapterOptions,
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
        &this->m_Adapter);

    WAIT_FOR_FUTURE(future);


    wgpu::AdapterInfo adapterInfo;
    this->m_Adapter.GetInfo(&adapterInfo);

    printf("Adapter name: %s\n", adapterInfo.device);

    wgpu::DeviceDescriptor deviceDescriptor;
    deviceDescriptor.label = "Default Device";
    deviceDescriptor.defaultQueue.label = "Default Queue";
    deviceDescriptor.deviceLostCallbackInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
    deviceDescriptor.deviceLostCallbackInfo.callback = [](const WGPUDevice* device, WGPUDeviceLostReason reason,
                                                char const* message, void*) {
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

    future = this->m_Adapter.RequestDevice(
        &deviceDescriptor,
        wgpu::CallbackMode::WaitAnyOnly,
        [](wgpu::RequestDeviceStatus status, wgpu::Device device,
           const char *message, wgpu::Device *userdata) {
            if (status != wgpu::RequestDeviceStatus::Success) {
                fprintf(stderr, "Failed to request device: %d\n", status);
                if (message) {
                    fprintf(stderr, "Message: %s\n", message);
                }
                return;
            }
            *userdata = std::move(device);
            printf("WebGPU Device initialized\n");
        },
        &this->m_Device);

    WAIT_FOR_FUTURE(future);


    this->m_Surface.GetCapabilities(this->m_Adapter, &this->m_SurfaceCapabilities);
    this->m_TextureFormat = this->m_SurfaceCapabilities.formats[0];
    this->m_Queue = this->m_Device.GetQueue();





    wgpu::SurfaceConfiguration surfaceConfiguration{
        .device = this->m_Device,
        .format = this->m_TextureFormat,
        .width = width,
        .height = height,
    };

    this->m_Surface.Configure(&surfaceConfiguration);

    glfwSetWindowUserPointer(this->m_Window, this);

    printf("WebGPU Context initialized\n");
}

WebGPU::~WebGPU() {
    if (m_Instance) {
        printf("Destroying WebGPU Context\n");
        m_Instance = nullptr;
        m_Adapter = nullptr;
        m_Device = nullptr;
        m_Queue = nullptr;
    }
}

void WebGPU::SetVisible(bool show) {
    if (show) {
        glfwShowWindow(this->m_Window);
    } else {
        glfwHideWindow(this->m_Window);
    }
}
