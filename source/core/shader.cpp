#include "core/shader.hpp"
#include "core/webgpu.hpp"

#include <fstream>
#include <sstream>


Shader::Shader(const std::string &label, const std::string &filePath) {
    this->m_Label = label;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::stringstream stream;
    stream << file.rdbuf();
    this->m_Source = stream.str();

    file.close();

}

wgpu::ShaderModule Shader::Transfer(WebGPU &webgpu) const {

    wgpu::ShaderModuleWGSLDescriptor shaderModuleWGSLDescriptor{};
    shaderModuleWGSLDescriptor.code = m_Source.c_str();
    wgpu::ShaderModuleDescriptor shaderModuleDescriptor{
        .nextInChain = &shaderModuleWGSLDescriptor,
    };
    shaderModuleDescriptor.label = m_Label.c_str();

    wgpu::ShaderModule shaderModule = webgpu.m_Device.CreateShaderModule(&shaderModuleDescriptor);

    return shaderModule;

}
