#pragma once
#include "core/Ref.hpp"
#include "Chunk.hpp"
#include <glm/glm.hpp>
#include <map>


namespace std {
    template<>
    struct hash<glm::vec3> {
        std::size_t operator()(const glm::vec3 &v) const {
            std::size_t seed = 0;
            // Combine the hashes of the vector's components
            seed ^= std::hash<float>{}(v.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<float>{}(v.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<float>{}(v.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

class World {
public:
    void Init();

    void Generate();
    void BuildFullMesh();



    uint32_t GetBlock(int x, int y, int z);

    std::unordered_map<glm::vec3, Ref<Chunk>> m_Chunks;



};
