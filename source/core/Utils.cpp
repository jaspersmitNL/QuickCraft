#include "core/Utils.hpp"
#include <webgpu/webgpu_cpp.h>
#include <sstream>
#include <fstream>

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

    wgpu::ShaderModule LoadShaderFromFile(wgpu::Device &device, const char *path, const char *label) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(path));
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return LoadShader(device, buffer.str(), label);

    }
}
