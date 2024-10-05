#pragma once
#include <webgpu/webgpu_cpp.h>

namespace Core {
#define WAIT_FOR_FUTURE(instance, future) instance.WaitAny(future, UINT64_MAX)


        template<typename T>
        const T *ToPtr(const T &&value) {
            return &value;
        }

    wgpu::ShaderModule LoadShader(wgpu::Device& device, std::string source, const char * label);

}
