#pragma once
#include <webgpu/webgpu_cpp.h>
#include "core/webgpu.hpp"




template<typename T>
class Buffer {
public:
    WebGPU &m_WebGPU;
    const char *m_Label;
    wgpu::Buffer m_Buffer;


    Buffer(WebGPU &webgpu, const char *label) :m_WebGPU(webgpu), m_Label(label) {
    }

    Buffer(WebGPU &webgpu, const char *label, const std::vector<T> &data, wgpu::BufferUsage usage)
        :m_WebGPU(webgpu), m_Label(label)
    {
        Transfer(data, usage);
    }


    void Transfer(const std::vector<T> &data, wgpu::BufferUsage usage) {
        wgpu::BufferDescriptor bufferDescriptor{
            .label = this->m_Label,
            .usage = usage,
            .size = sizeof(T) * data.size(),
            .mappedAtCreation = true,
        };

        size_t size = data.size();
        this->m_Buffer = this->m_WebGPU.m_Device.CreateBuffer(&bufferDescriptor);
        void *bufferData = this->m_Buffer.GetMappedRange(0, sizeof(T) * size);
        memcpy(bufferData, data.data(), sizeof(T) * size);
        this->m_Buffer.Unmap();
    }

    void Write(const T &data, size_t offset = 0) {
        this->m_WebGPU.m_Queue.WriteBuffer(this->m_Buffer, offset, &data, sizeof(T));
    }

    void Write(const std::vector<T> &data, size_t offset = 0) {
        this->m_WebGPU.m_Queue.WriteBuffer(this->m_Buffer, offset * sizeof(T), data.data(), data.size() * sizeof(T));
    }
};
