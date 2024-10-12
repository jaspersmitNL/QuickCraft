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
            this->m_Chunks.push_back(CreateRef<Chunk>(glm::ivec3{x, 0, z}));
        }
    }
}

void World::Generate() {
    for (auto& chunk : m_Chunks) {
        chunk->Generate();
    }
}

std::vector<std::thread> threads;

void World::BuildFullMesh() {
    for (auto& chunk : m_Chunks) {
        chunk->BuildMesh();
    }



}
