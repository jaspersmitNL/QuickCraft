//
// Created by jasper on 13-10-2024.
//

#include "Raycast.hpp"
#include "game/MiniCraft.hpp"
#include "game/world/Chunk.hpp"
#include "game/world/World.hpp"

#define STEP_SIZE 0.01f

RayCastResult Raycast::Cast(glm::vec3 origin, glm::vec3 direction, float maxDistance) {

    auto& mc = MiniCraft::Get();
    auto& world = mc->m_World;

    RayCastResult result{
        .didHit = false,
        .distance = 0,
        .hitPos = glm::vec3(0),
        .blockPos = glm::vec3(0),
        .chunk = nullptr
    };

    /*
    *    auto camPos = m_Camera->GetPosition();


    glm::vec3 chunkPos = glm::vec3{std::floor(camPos.x / CHUNK_SIZE), 0, std::floor(camPos.z / CHUNK_SIZE)};
    auto chunk = m_World->m_Chunks[chunkPos];
    if (!chunk) {
    printf("Failed to find chunk at position: %f, %f\n", chunkPos.x, chunkPos.z);
    return;
    }

    glm::vec3 blockPos = glm::vec3{camPos.x - chunkPos.x * CHUNK_SIZE, camPos.y, camPos.z - chunkPos.z * CHUNK_SIZE};

    printf("Found chunk at position: %f, %f\n", chunkPos.x, chunkPos.z);

    chunk->SetBlock(blockPos.x, blockPos.y, blockPos.z, 1); // Grass

    chunk->BuildMesh();
    */


    for (float i = 0; i < maxDistance; i += STEP_SIZE) {
        glm::vec3 position = origin + direction * i;

        int chunkX = position.x >= 0 ? position.x / CHUNK_SIZE : position.x / CHUNK_SIZE - 1;
        int chunkY = 0;
        int chunkZ = position.z >= 0 ? position.z / CHUNK_SIZE : position.z / CHUNK_SIZE - 1;

        auto& chunk = world->m_Chunks[{chunkX, chunkY, chunkZ}];

        if (!chunk) {
            continue;
        }

        // get the block position in the chunk
        glm::vec3 blockPos = {
            std::floor(position.x - chunkX * CHUNK_SIZE),
            std::floor(position.y),
            std::floor(position.z - chunkZ * CHUNK_SIZE)
        };

        uint32_t blockID = chunk->GetBlock(blockPos.x, blockPos.y, blockPos.z);


        result.distance = i;
        result.hitPos = position;
        result.blockPos = blockPos;
        result.chunk = chunk;

        if (blockID != 0) {
            result.didHit = true;
            break;
        }



    }




    return result;
}
