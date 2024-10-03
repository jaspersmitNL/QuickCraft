#include "core/Input.hpp"
#include "core/webgpu.hpp"
#include <GLFW/glfw3.h>

bool Input::IsKeyDown(KeyCode keycode) {
    int state = glfwGetKey(WebGPU::m_Window, (int)keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonDown(MouseButton button) {
    int state = glfwGetMouseButton(WebGPU::m_Window, (int)button);
    return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition() {
    double x, y;
    glfwGetCursorPos(WebGPU::m_Window, &x, &y);
    return { (float)x, (float)y };
}

void Input::SetCursorMode(CursorMode mode) {
    glfwSetInputMode(WebGPU::m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}