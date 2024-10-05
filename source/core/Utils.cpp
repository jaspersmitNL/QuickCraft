#include "core/Utils.hpp"
#include <webgpu/webgpu_cpp.h>


namespace Core {
    wgpu::ShaderModule LoadShader(wgpu::Device &device, std::string source, const char * label) {
        wgpu::ShaderModuleWGSLDescriptor shaderModuleWgslDescriptor;
        shaderModuleWgslDescriptor.code = source.c_str();

        wgpu::ShaderModuleDescriptor shaderModuleDescriptor{
            .nextInChain = &shaderModuleWgslDescriptor,
            .label = label,
        };

        return device.CreateShaderModule(&shaderModuleDescriptor);
    }
}
