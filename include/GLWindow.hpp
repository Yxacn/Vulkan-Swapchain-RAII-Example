// GLWindow.hpp
// GLFW 窗口封装：创建无 OpenGL 上下文的窗口，对外提供关闭/最小化状态查询。
// GLFW 全局状态以引用计数维护——首个窗口初始化 GLFW，最后一个窗口销毁时终止，
// 多个 GLWindow 实例可安全共存；窗口固定尺寸并自动居中到主显示器。
#pragma once

#include <string>
#include <string_view>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vkp
{
    inline constexpr int DEFAULT_WINDOW_WIDTH = 800;
    inline constexpr int DEFAULT_WINDOW_HEIGHT = 600;
    inline constexpr std::string_view DEFAULT_WINDOW_TITLE = "Vulkan";

    struct WindowInfo
    {
        int width{ DEFAULT_WINDOW_WIDTH };
        int height{ DEFAULT_WINDOW_HEIGHT };
        std::string title{ DEFAULT_WINDOW_TITLE };
    };

    class GLWindow
    {
    public:
        explicit GLWindow(const WindowInfo& windowInfo = {});
        ~GLWindow();

        GLWindow(GLWindow&& other) noexcept;
        GLWindow& operator=(GLWindow&& other) noexcept;

        GLWindow(const GLWindow&) = delete;
        GLWindow& operator=(const GLWindow&) = delete;

        void createWindow();
        [[nodiscard]] GLFWwindow* getWindowInstance() const;
        [[nodiscard]] bool shouldClose() const;
        [[nodiscard]] bool isMinimized() const; // 最小化时帧缓冲尺寸为 0，调用方据此跳过绘制

    private:
        void initWindow();
        void centerWindow();
        void destroyWindow() noexcept;

    private:
        WindowInfo m_windowInfo;
        GLFWwindow* m_window{ nullptr };

        static int sm_glfwRefCount; // GLFW 全局引用计数
    };
} // namespace vkp
