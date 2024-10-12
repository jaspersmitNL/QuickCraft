#include "World.hpp"
#include <chrono>



auto NOW() {
    return std::chrono::high_resolution_clock::now();
}

auto GetDuration(std::chrono::time_point<std::chrono::high_resolution_clock> start, std::chrono::time_point<std::chrono::high_resolution_clock> end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

void World::Init() {



    for(int x = -4; x <= 4;x++) {
        for(int z = -4; z <= 4; z++) {
            auto pos = glm::vec3{x, 0, z};
            this->m_Chunks[pos] = CreateRef<Chunk>(pos);
        }
    }
}

void World::Generate() {
    for (auto& [key, chunk] : m_Chunks) {
        chunk->Generate();
    }
}


void World::BuildFullMesh() {
    for (auto& [key, chunk] : m_Chunks) {
        chunk->BuildMesh();
    }



}

uint32_t World::GetBlock(int x, int y, int z) {

    // chunks are in a grid from -4 to 4
    glm::vec3 chunkPos = glm::vec3{std::floor(x / CHUNK_SIZE), std::floor(y / CHUNK_SIZE), std::floor(z / CHUNK_SIZE)};

    // blocks in the chunk are from 0 to CHUNK_SIZE
    glm::vec3 blockPos = glm::vec3{x % CHUNK_SIZE, y % CHUNK_SIZE, z % CHUNK_SIZE};


    auto chunk = m_Chunks[chunkPos];
    if (chunk) {
        return chunk->GetBlock(blockPos.x, blockPos.y, blockPos.z);
    }
    return 0;

}
