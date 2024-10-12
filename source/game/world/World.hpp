#pragma once
#include "core/Ref.hpp"
#include "Chunk.hpp"
#include <map>

class World {
public:
    void Init();

    void Generate();
    void BuildFullMesh();



    std::vector<Ref<Chunk>> m_Chunks;



};
