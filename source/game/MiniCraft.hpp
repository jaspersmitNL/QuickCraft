#pragma once
#include "Camera.hpp"
#include "core/Ref.hpp"
#include "core/Context.hpp"


class WorldRenderer;
class World;
class MiniCraft {

public:
    MiniCraft();


    void OnRender();

    void Run();

    ~MiniCraft();

    void Test();


    Ref<Core::Context> m_RenderContext;
    Ref<Camera> m_Camera;
    Ref<World> m_World;
    Ref<WorldRenderer> m_WorldRenderer;

    inline static Ref<MiniCraft> m_Instance;
    static Ref<MiniCraft>& Get() { return m_Instance; }


private:
    void Init();


};



