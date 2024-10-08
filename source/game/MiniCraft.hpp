#pragma once
#include "Ref.hpp"
#include "Camera.hpp"
#include "core/Context.hpp"

class MiniCraft {

public:
    MiniCraft();


    void OnRender();

    void Run();

    ~MiniCraft();


    Ref<Core::Context> m_RenderContext;
    Ref<Camera> m_Camera;

    inline static Ref<MiniCraft> m_Instance;
    static Ref<MiniCraft>& Get() { return m_Instance; }

};



