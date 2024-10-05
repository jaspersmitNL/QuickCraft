#pragma once
#include <webgpu/webgpu_cpp.h>


template<typename T>
const T *ToPtr(const T &&value) {
    return &value;
}



namespace Core {
#define WAIT_FOR_FUTURE(instance, future) instance.WaitAny(future, UINT64_MAX)



    wgpu::ShaderModule LoadShader(wgpu::Device& device, std::string source, const char * label);

    wgpu::ShaderModule LoadShaderFromFile(wgpu::Device& device, const char * path, const char * label);
    wgpu::Buffer CreateBufferFromData(wgpu::Device& device, wgpu::BufferUsage usage, const void* data, uint64_t size);

}
