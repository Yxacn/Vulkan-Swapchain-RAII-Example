// Application.cpp
// 应用层实现：构造先建窗口后建引擎，移动赋值先移交引擎再移交窗口，
// 保证旧引擎销毁依赖窗口的交换链/表面时旧窗口尚未释放。
// 主循环用 glfwWaitEvents 阻塞等待事件，静态场景下避免空转轮询。
#include "Application.hpp"

#include <utility>

namespace vkp
{
    Application::Application(const WindowInfo& windowInfo, const VkApplicationInfo& appInfo,
                             const VkInstanceCreateInfo& instanceCreateInfo)
        : m_glw(windowInfo)
        , m_vke(m_glw.getWindowInstance(), appInfo, instanceCreateInfo)
    {
    }

    Application::~Application() {}

    Application::Application(Application&& other) noexcept
        : m_glw(std::move(other.m_glw))
        , m_vke(std::move(other.m_vke))
    {
    }

    Application& Application::operator=(Application&& other) noexcept
    {
        if (this != &other)
        {
            // 先移交引擎再移交窗口：旧引擎析构时销毁的交换链/表面依赖旧窗口，
            // 若窗口先被销毁，引擎清理阶段将引用失效的窗口句柄
            m_vke = std::move(other.m_vke);
            m_glw = std::move(other.m_glw);
        }
        return *this;
    }

    // glfwWaitEvents 无事件时阻塞线程：静态场景下比轮询更省 CPU；
    // 若加入动画需要每帧重绘，应改用 glfwPollEvents
    void Application::run()
    {
        while (!m_glw.shouldClose())
        {
            // 最小化时帧缓冲尺寸为 0，跳过绘制；恢复时收到事件唤醒并自动重建交换链
            if (!m_glw.isMinimized())
            {
                m_vke.drawFrame();
            }
            glfwWaitEvents();
        }
    }
} // namespace vkp
