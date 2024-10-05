#pragma once
#include <webgpu/webgpu_cpp.h>


template<typename T>
const T *ToPtr(const T &&value) {
    return &value;
}

namespace Core {
#define WAIT_FOR_FUTURE(instance, future) instance.WaitAny(future, UINT64_MAX)


    struct BindingLayoutEntryInitializationHelper : wgpu::BindGroupLayoutEntry {
        BindingLayoutEntryInitializationHelper(uint32_t entryBinding,
                                               wgpu::ShaderStage entryVisibility,
                                               wgpu::BufferBindingType bufferType,
                                               bool bufferHasDynamicOffset = false,
                                               uint64_t bufferMinBindingSize = 0);
        BindingLayoutEntryInitializationHelper(uint32_t entryBinding,
                                               wgpu::ShaderStage entryVisibility,
                                               wgpu::SamplerBindingType samplerType);
        BindingLayoutEntryInitializationHelper(
            uint32_t entryBinding,
            wgpu::ShaderStage entryVisibility,
            wgpu::TextureSampleType textureSampleType,
            wgpu::TextureViewDimension viewDimension = wgpu::TextureViewDimension::e2D,
            bool textureMultisampled = false);
        BindingLayoutEntryInitializationHelper(
            uint32_t entryBinding,
            wgpu::ShaderStage entryVisibility,
            wgpu::StorageTextureAccess storageTextureAccess,
            wgpu::TextureFormat format,
            wgpu::TextureViewDimension viewDimension = wgpu::TextureViewDimension::e2D);
        BindingLayoutEntryInitializationHelper(const wgpu::BindGroupLayoutEntry& entry);
    };


    wgpu::ShaderModule LoadShader(wgpu::Device& device, std::string source, const char * label);

    wgpu::ShaderModule LoadShaderFromFile(wgpu::Device& device, const char * path, const char * label);




    wgpu::Buffer CreateBufferFromData(wgpu::Device& device, wgpu::BufferUsage usage, const void* data, uint64_t size);

}
