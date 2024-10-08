//
// Created by jasper on 8-10-2024.
//

#include "Chunk.hpp"

uint32_t Chunk::GetBlock(int x, int y, int z) {
    const int idx = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return 0;
    }
    return m_Blocks[idx];
}

void Chunk::SetBlock(int x, int y, int z, uint32_t block) {
    const int idx = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return;
    }
    m_Blocks[idx] = block;
}

void Chunk::Generate() {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                uint32_t blockID = (x + y + z) % 2 == 0 ? 1 : 2;
                SetBlock(x, y, z, blockID);
            }
        }
    }
}
