#include "Input.hpp"
#include "core/Context.hpp"
#include <GLFW/glfw3.h>

namespace Core {

    bool Input::IsKeyDown(KeyCode keycode) {
        int state = glfwGetKey(Context::m_Window, (int)keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonDown(MouseButton button) {
        int state = glfwGetMouseButton(Context::m_Window, (int)button);
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePosition() {
        double x, y;
        glfwGetCursorPos(Context::m_Window, &x, &y);
        return { (float)x, (float)y };
    }

    void Input::SetCursorMode(CursorMode mode) {
        glfwSetInputMode(Context::m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
    }
}