#include "WorldRenderer.hpp"
#include "core/Utils.hpp"
#include "core/builder/RenderPipelineBuilder.hpp"
#include "game/MiniCraft.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "imgui.h"
#include "stb_image.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


#define TEXTURE_SIZE 16
#define RENDER_SIZE 8



unsigned char *LoadImage(const char *path) {
    int width, height, channels;
    unsigned char *pixels = stbi_load(path, &width, &height, &channels, 0);
    if (!pixels) {
        printf("Failed to load image: %s\n", path);
        exit(1);
    }
    if (channels == 3) {
        unsigned char *pixels4 = new unsigned char[width * height * 4];
        for (int i = 0; i < width * height; i++) {
            pixels4[i * 4] = pixels[i * 3];
            pixels4[i * 4 + 1] = pixels[i * 3 + 1];
            pixels4[i * 4 + 2] = pixels[i * 3 + 2];
            pixels4[i * 4 + 3] = 255;
        }
        printf("Converted to 4 channels\n");
        delete[] pixels;
        pixels = std::move(pixels4);
    }

    printf("Loaded texture %s width: %d, height: %d, channels: %d\n", path, width, height, channels);

    return pixels;
}


void WorldRenderer::LoadTextures(std::vector<TextureObject> textures) {
    auto &ctx = MiniCraft::Get()->m_RenderContext;
    uint32_t num = textures.size() + 1; // +1 for the missing texture of block 0
    wgpu::TextureDescriptor textureDescriptor{
        .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst |
                 wgpu::TextureUsage::RenderAttachment,
        .size = {TEXTURE_SIZE, TEXTURE_SIZE, num},
        .format = wgpu::TextureFormat::RGBA8Unorm,
    };

    m_Texture = ctx->m_Device.CreateTexture(&textureDescriptor);

    for (auto &texture: textures) {
        wgpu::ImageCopyTexture destination;
        destination.texture = m_Texture;
        destination.mipLevel = 0;
        destination.origin = {0, 0, texture.idx};
        destination.aspect = wgpu::TextureAspect::All;

        wgpu::TextureDataLayout source;
        source.offset = 0;
        source.bytesPerRow = TEXTURE_SIZE * 4;
        source.rowsPerImage = TEXTURE_SIZE;

        wgpu::Extent3D copySize = {TEXTURE_SIZE, TEXTURE_SIZE, 1};
        unsigned char *pixels = LoadImage(texture.path.c_str());
        ctx->m_Queue.WriteTexture(&destination, pixels, 4 * TEXTURE_SIZE * TEXTURE_SIZE, &source, &copySize);

        delete[] pixels;

        printf("Loaded texture %s\n", texture.path.c_str());
    }

    wgpu::SamplerDescriptor samplerDescriptor;
    samplerDescriptor.minFilter = wgpu::FilterMode::Nearest;
    samplerDescriptor.magFilter = wgpu::FilterMode::Nearest;
    m_Sampler = ctx->m_Device.CreateSampler(&samplerDescriptor);
}


void WorldRenderer::Init() {
    auto &ctx = MiniCraft::Get()->m_RenderContext;

    MiniCraft::Get()->m_Camera->SetPosition(glm::vec3(0, 25, 0));

    m_VertexBuffer = Core::CreateBufferFromData(ctx->m_Device, wgpu::BufferUsage::Vertex, Chunk::blockVertices.data(),
                                                sizeof(BlockVertex) * Chunk::blockVertices.size());


    m_UniformBuffer = Core::CreateBufferFromData(ctx->m_Device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
                                                 &m_Uniforms, sizeof(Uniforms));

    m_Shader = Core::LoadShaderFromFile(ctx->m_Device, "../res/world.wgsl", "World Shader");


    LoadTextures({
        {1, "../res/grass_top.png"},
        {2, "../res/grass_side.png"},
        {3, "../res/dirt.png"},
        {4, "../res/cobble.png"},
        {5, "../res/stone.png"},
        {6, "../res/bedrock.png"},
        {7, "../res/missing.png"},
    });

    Core::RenderPipelineBuilder builder(ctx);
    // @formatter:off
    m_Pipeline = builder
        .SetVertexEntryPoint("vs_main")
        .SetFragmentEntryPoint("fs_main")
        .SetShaderModule(m_Shader)
        .AddVertexBufferLayout({
            {
                .format = wgpu::VertexFormat::Float32x3,
                .offset = offsetof(BlockVertex, position),
                .shaderLocation = 0,
            },
            {
                .format = wgpu::VertexFormat::Float32x2,
                .offset = offsetof(BlockVertex, uv),
                .shaderLocation = 1,
            }
        }, sizeof(BlockVertex), wgpu::VertexStepMode::Vertex)
        .AddVertexBufferLayout({
            {
                .format = wgpu::VertexFormat::Float32x3,
                .offset = offsetof(BlockFace, center),
                .shaderLocation = 2,
            },
            {
                .format = wgpu::VertexFormat::Uint32,
                .offset = offsetof(BlockFace, orientation),
                .shaderLocation = 3,
            },
            {
                .format = wgpu::VertexFormat::Uint32,
                .offset = offsetof(BlockFace, blockID),
                .shaderLocation = 4,
            }
        }, sizeof(BlockFace), wgpu::VertexStepMode::Instance)
        .EnableDepthStencil(true)
        .Build();
    // @formatter:on


    wgpu::BindGroupDescriptor bindGroupDescriptor{};
    bindGroupDescriptor.layout = m_Pipeline.GetBindGroupLayout(0);
    std::vector<wgpu::BindGroupEntry> bindGroupEntries = {
        {
            .binding = 0,
            .buffer = m_UniformBuffer,
        },
        {
            .binding = 1,
            .sampler = m_Sampler,
        },
        {
            .binding = 2,
            .textureView = m_Texture.CreateView()
        }
    };

    bindGroupDescriptor.entryCount = bindGroupEntries.size();
    bindGroupDescriptor.entries = bindGroupEntries.data();

    m_BindGroup = MiniCraft::Get()->m_RenderContext->m_Device.CreateBindGroup(&bindGroupDescriptor);


    printf("WorldRenderer initialized\n");
}

void WorldRenderer::Render(wgpu::CommandEncoder &commandEncoder, wgpu::SurfaceTexture &surfaceTexture) {
    auto &ctx = MiniCraft::Get()->m_RenderContext;
    auto &camera = MiniCraft::Get()->m_Camera;


    ImGui::Begin("WorldRenderer");
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("FPS: %.2f", io.Framerate);
    ImGui::Text("Camera Position: %.2f %.2f %.2f", camera->GetPosition().x, camera->GetPosition().y,
                camera->GetPosition().z);
    ImGui::InputFloat("Fog Near", &m_Uniforms.fogNear, 0.1f, 1.0f);
    ImGui::InputFloat("Fog Far", &m_Uniforms.fogFar, 0.1f, 1.0f);
    ImGui::End();


    //nice sky like blue

    wgpu::RenderPassColorAttachment colorAttachment{
        .view = surfaceTexture.texture.CreateView(),
        .loadOp = wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store,
    };

    wgpu::RenderPassDepthStencilAttachment renderPassDepthStencilAttachment{
        .view = ctx->m_DepthTexture.CreateView(),
        .depthLoadOp = wgpu::LoadOp::Load,
        .depthStoreOp = wgpu::StoreOp::Store
    };

    wgpu::RenderPassDescriptor renderPassDescriptor{
        .label = "World Render Pass",
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachment,
        .depthStencilAttachment = &renderPassDepthStencilAttachment,
    };

    wgpu::RenderPassEncoder pass = commandEncoder.BeginRenderPass(&renderPassDescriptor);
    auto camPos = camera->GetPosition();

    pass.SetPipeline(m_Pipeline);
    pass.SetVertexBuffer(0, m_VertexBuffer);
    pass.SetBindGroup(0, m_BindGroup);


    m_Uniforms.projection = camera->GetProjection();
    m_Uniforms.view = camera->GetView();
    m_Uniforms.cameraPosition = {camPos.x, camPos.y, camPos.z, 1.0f};
    m_Uniforms.inverseProjection =camera->GetInverseProjection();
    m_Uniforms.inverseView = camera->GetInverseView();



    ctx->m_Device.GetQueue().WriteBuffer(m_UniformBuffer, 0, &m_Uniforms, sizeof(Uniforms));



    //render a 4x4 chunks around the camera
    glm::ivec3 start = {
        (int) (camPos.x / CHUNK_SIZE) - RENDER_SIZE / 2,
        (int) (camPos.y / CHUNK_SIZE) - RENDER_SIZE / 2,
        (int) (camPos.z / CHUNK_SIZE) - RENDER_SIZE / 2
    };

    for (int x = start.x; x < start.x + RENDER_SIZE; x++) {
        for (int z = start.z; z < start.z + RENDER_SIZE; z++) {
            int y = 0;
            //world m_Chunks is a map of chunks, so we can just get the chunk at the position
            auto chunk = m_World->m_Chunks[{x, y, z}];
            if (!chunk) {
                chunk = CreateRef<Chunk>(glm::vec3(x, y, z));
                m_World->m_Chunks[{x, y, z}] = chunk;
                chunk->Generate();
                chunk->BuildMesh();
            }
            if (!chunk->m_IsReady) continue;


            pass.SetVertexBuffer(1, chunk->m_InstanceBuffer);
            pass.Draw(6, chunk->m_VertexCount, 0, 0);
        }
    }


    pass.End();
}
