#include <chrono>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
#include <vector>
#include <future>
#include <iostream>
#include <mutex>

#include "World.hpp"
#include "core/Utils.hpp"
#include "game/Camera.hpp"
#include "FastNoiseLite.hpp"


#include "imgui.h"
#include "GLFW/glfw3.h"

#define SIZE 64


auto GetBlock = [&](int x, int y, int z, const std::vector<unsigned int>& chunkData)-> uint32_t {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || z < 0 || z >= SIZE) {
        return 0;
    }
    uint32_t idx = x + z * SIZE + y * SIZE * SIZE;
    return chunkData[idx];
};


int count = 0;
std::mutex faceMutex;

enum Face: uint32_t {
    FRONT = 0,
    BACK = 1,
    LEFT = 2,
    RIGHT = 3,
    TOP = 4,
    BOTTOM = 5,
};


void MeshSection(int startX, int endX, int startZ, int endZ, std::vector<BlockFace>& faces, const std::vector<unsigned int>& chunkData) {
    for (int x = startX; x < endX; x++) {
        for (int z = startZ; z < endZ; z++) {
            for (int y = 0; y < SIZE; y++) {

                uint32_t blockID = GetBlock(x, y, z, chunkData);

                if (blockID == 0) {
                    continue;
                }

                // Perform face culling as before (checking neighbors)
                if (GetBlock(x, y, z+1, chunkData) == 0) {
                    std::lock_guard<std::mutex> lock(faceMutex); // Lock if using shared face vector
                    faces.push_back({{x, y, z}, FRONT, blockID});
                }
                if (GetBlock(x, y, z-1, chunkData) == 0) {
                    std::lock_guard<std::mutex> lock(faceMutex);
                    faces.push_back({{x, y, z}, BACK, blockID});
                }
                if (GetBlock(x+1, y, z, chunkData) == 0) {
                    std::lock_guard<std::mutex> lock(faceMutex);
                    faces.push_back({{x, y, z}, LEFT, blockID});
                }
                if (GetBlock(x-1, y, z, chunkData) == 0) {
                    std::lock_guard<std::mutex> lock(faceMutex);
                    faces.push_back({{x, y, z}, RIGHT, blockID});
                }
                if (GetBlock(x, y+1, z, chunkData) == 0) {
                    std::lock_guard<std::mutex> lock(faceMutex);
                    faces.push_back({{x, y, z}, TOP, blockID});
                }
                if (GetBlock(x, y-1, z, chunkData) == 0) {
                    std::lock_guard<std::mutex> lock(faceMutex);
                    faces.push_back({{x, y, z}, BOTTOM, blockID});
                }

            }
        }
    }
}
void World::Initialize(Core::Context &ctx) {
    std::vector<BlockFaceVert> vertices = {
        {glm::vec3(-0.5f, -0.5f, 0.5f)}, // bottom left
        {glm::vec3(0.5f, -0.5f, 0.5f)}, // bottom right
        {glm::vec3(0.5f, 0.5f, 0.5f)}, // top right
        {glm::vec3(0.5f, 0.5f, 0.5f)}, // top right
        {glm::vec3(-0.5f, 0.5f, 0.5f)}, // top left
        {glm::vec3(-0.5f, -0.5f, 0.5f)}, // bottom left
    };


    std::vector<BlockFace> AllFaces = {
        {{0.0f, 0.0f, 0.0f}, 0}, //front
        {{0.0f, 0.0f, 0.0f}, 1}, // back
        {{0.0f, 0.0f, 0.0f}, 2}, //left
        {{0.0f, 0.0f, 0.0f}, 3}, //right
        {{0.0f, 0.0f, 0.0f}, 4}, //top
        {{0.0f, 0.0f, 0.0f}, 5}, //bottom
    };

    std::vector<BlockFace> faces;



    int seed = 1337;

    FastNoiseLite terrain;
    terrain.SetSeed(seed);
    terrain.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    terrain.SetFrequency(0.001f);

    FastNoiseLite forest;
    forest.SetSeed(seed);
    forest.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    forest.SetFractalType(FastNoiseLite::FractalType_Ridged);


    FastNoiseLite mountain;
    mountain.SetSeed(seed);
    mountain.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    mountain.SetFractalType(FastNoiseLite::FractalType_Ridged);
    mountain.SetFrequency(0.005f);


    FastNoiseLite river;
    river.SetSeed(seed);
    river.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    river.SetFractalType(FastNoiseLite::FractalType_Ridged);
    river.SetFrequency(0.001f);



    auto getHeight = [&](float x, float z) {
        int height;

        float noise = abs(terrain.GetNoise(x, z) + 0.5f);
        if (noise < 0.05f)
        {
            height = (int)abs(8 * (river.GetNoise(x, z) + 0.8f)) + 30;
        }

        else if (noise < 1.2f)
        {
            height = (int)abs(10 * (forest.GetNoise(x, z) + 0.8f)) + 30;
        }

        else
        {
            height = (int)abs(30 * (mountain.GetNoise(x, z) + 0.8f)) + 30;
        }

        return height;
    };


    std::vector<unsigned int> chunkData(SIZE * SIZE * SIZE);



    auto start = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < SIZE; x++) {
        for (int z = 0; z < SIZE; z++) {
            int height = getHeight(x, z);
            for(int y = 0; y < height; y++) {
                uint32_t blockID = (x + y + z) % 2 == 0 ? 1 : 2;
                uint32_t idx = x + z * SIZE + y * SIZE * SIZE;
                chunkData[idx] = blockID;
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Generation took: " << duration.count() << "ms" << std::endl;





    start = std::chrono::high_resolution_clock::now();


    int batchCount = 12;

    std::vector<std::vector<BlockFace>> faceSections(batchCount);

    // Calculate the size of each chunk based on the batch count
    int chunkSize = SIZE / batchCount;

    // Vector to store the future objects for each thread
    std::vector<std::future<void>> futures;

    // Launch threads dynamically
    for (int i = 0; i < batchCount; i++) {
        int startX = i * chunkSize;
        int endX = (i == batchCount - 1) ? SIZE : (i + 1) * chunkSize;  // Ensure last chunk covers the remainder

        futures.push_back(
            std::async(std::launch::async, MeshSection, startX, endX, 0, SIZE, std::ref(faceSections[i]), std::ref(chunkData))
        );
    }

    // Wait for all threads to complete
    for (auto& future : futures) {
        future.get();
    }

    // Combine results into the final face vector
    for (const auto& sectionFaces : faceSections) {
        faces.insert(faces.end(), sectionFaces.begin(), sectionFaces.end());
    }

    count = faces.size();

    // for (int x = 0; x < SIZE; x++) {
    //     for (int z = 0; z < SIZE; z++) {
    //         for(int y = 0; y < SIZE; y++) {
    //
    //             uint32_t blockID = GetBlock(x, y, z);
    //
    //             if (blockID == 0) {
    //                 continue;
    //             }
    //
    //             if (GetBlock(x, y, z+1) == 0) {
    //                 faces.push_back({{x, y, z}, FRONT, blockID});
    //             }
    //             if (GetBlock(x, y, z-1) == 0) {
    //                 faces.push_back({{x, y, z}, BACK, blockID});
    //             }
    //             if (GetBlock(x+1, y, z) == 0) {
    //                 faces.push_back({{x, y, z}, LEFT, blockID});
    //             }
    //             if (GetBlock(x-1, y, z) == 0) {
    //                 faces.push_back({{x, y, z}, RIGHT, blockID});
    //             }
    //             if (GetBlock(x, y+1, z) == 0) {
    //                 faces.push_back({{x, y, z}, TOP, blockID});
    //             }
    //             if (GetBlock(x, y-1, z) == 0) {
    //                 faces.push_back({{x, y, z}, BOTTOM, blockID});
    //             }
    //         }
    //     }
    // }

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Meshing took: " << duration.count() << "ms" << std::endl;


    count = faces.size();

    m_VertexBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, vertices.data(),
                                                sizeof(BlockFaceVert) * vertices.size());

    m_InstanceBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Vertex, faces.data(),
                                                  sizeof(BlockFace) * faces.size());

    m_UniformBuffer = Core::CreateBufferFromData(ctx.m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
                                                 &m_Uniforms, sizeof(WorldUniforms));


    m_ShaderModule = Core::LoadShaderFromFile(ctx.m_Device, "../res/world.wgsl", "World");


    wgpu::ColorTargetState colorTargetState{
        .format = ctx.m_SurfaceTextureFormat,
    };

    wgpu::FragmentState fragmentState{
        .module = m_ShaderModule,
        .entryPoint = "fs_main",
        .targetCount = 1,
        .targets = &colorTargetState,
    };

    wgpu::VertexAttribute vertexAttributes[1] = {
        {
            .format = wgpu::VertexFormat::Float32x3,
            .offset = 0,
            .shaderLocation = 0,
        }
    };
    wgpu::VertexAttribute instanceAttributes[3] = {
        {
            .format = wgpu::VertexFormat::Float32x3,
            .offset = offsetof(BlockFace, center),
            .shaderLocation = 1,
        },
        {
            .format = wgpu::VertexFormat::Uint32,
            .offset = offsetof(BlockFace, orientation),
            .shaderLocation = 2,
        },
        {
            .format = wgpu::VertexFormat::Uint32,
            .offset = offsetof(BlockFace, blockID),
            .shaderLocation = 3,
        }
    };
    wgpu::VertexBufferLayout bufferLayouts[2] = {
        {
            .arrayStride = sizeof(BlockFaceVert),
            .stepMode = wgpu::VertexStepMode::Vertex,
            .attributeCount = 1,
            .attributes = vertexAttributes,
        },
        {
            .arrayStride = sizeof(BlockFace),
            .stepMode = wgpu::VertexStepMode::Instance,
            .attributeCount = 3,
            .attributes = instanceAttributes,
        }
    };

    wgpu::VertexState vertexState{
        .module = m_ShaderModule,
        .entryPoint = "vs_main",
        .bufferCount = 2,
        .buffers = bufferLayouts,
    };

    wgpu::DepthStencilState depthStencilState{
        .format = wgpu::TextureFormat::Depth24Plus,
        .depthWriteEnabled = {true},
        .depthCompare = wgpu::CompareFunction::Less,
    };

    wgpu::RenderPipelineDescriptor renderPipelineDescriptor{
        .label = "World Render Pipeline",
        .vertex = vertexState,
        .primitive = {
            .topology = wgpu::PrimitiveTopology::TriangleList,
        },
        .depthStencil = &depthStencilState,
        .fragment = &fragmentState,
    };

    m_Pipeline = ctx.m_Device.CreateRenderPipeline(&renderPipelineDescriptor);

    wgpu::BindGroupEntry bindGroupEntries[1] = {
        {
            .binding = 0,
            .buffer = m_UniformBuffer,
        },
    };
    wgpu::BindGroupDescriptor bindGroupDescriptor{
        .layout = m_Pipeline.GetBindGroupLayout(0),
        .entryCount = 1,
        .entries = bindGroupEntries,
    };

    m_BindGroup = ctx.m_Device.CreateBindGroup(&bindGroupDescriptor);
}

void World::Render(Core::Context &ctx, wgpu::CommandEncoder encoder, wgpu::SurfaceTexture &surfaceTexture,
                   wgpu::Texture &depthTexture, Camera &camera) {



    ImGui::Begin("Overlay");
    ImGui::Text("Count: %d", count);
    //fps
    static float fps = 0;
    static float lastTime = 0;
    float currentTime = glfwGetTime();
    fps = 1.0f / (currentTime - lastTime);
    lastTime = currentTime;
    ImGui::Text("FPS: %.2f", fps);

    ImGui::End();

    m_Uniforms.projection = camera.GetProjection();
    m_Uniforms.view = camera.GetView();

    ctx.m_Device.GetQueue().WriteBuffer(m_UniformBuffer, 0, &m_Uniforms, sizeof(WorldUniforms));


    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {0.0f, 0.0f, 0.0f, 1.0f},
    };

    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = depthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Load,
        .depthStoreOp = wgpu::StoreOp::Store
    };

    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "World Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachment,
        .depthStencilAttachment = &renderPassDepthStencilAttachment,
    };

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDescriptor);
    pass.SetPipeline(m_Pipeline);
    pass.SetBindGroup(0, m_BindGroup);
    pass.SetVertexBuffer(0, m_VertexBuffer);
    pass.SetVertexBuffer(1, m_InstanceBuffer);
    pass.Draw(6, count, 0, 0);
    pass.End();
}
