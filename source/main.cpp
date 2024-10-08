#include "game/MiniCraft.hpp"

int main() {

    MiniCraft::m_Instance = std::make_shared<MiniCraft>();


    MiniCraft::Get()->Run();

    return 0;
}
