#pragma once
#include <string>
#include <webgpu/webgpu_cpp.h>
#include "GLFW/glfw3.h"

class WebGPU;

class Shader {
public:
    std::string m_Label;
    std::string m_Source;

    Shader(const std::string& label, const std::string& filePath);


    wgpu::ShaderModule Transfer(WebGPU& webgpu) const;

};