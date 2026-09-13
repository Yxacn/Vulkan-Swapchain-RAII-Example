// CommandManager.hpp
// 命令管理：持有图形命令池，并按交换链图像数量分配、录制主命令缓冲。
// 命令缓冲与图像索引一一对应，交换链重建后整组释放并重新录制。
#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace vkp
{
    class VulkanContext;
    class SwapChain;
    class RenderPassPipeline;
    class FrameBufferManager;
    class BufferManager;

    class CommandManager
    {
    public:
        explicit CommandManager(VulkanContext& context);
        ~CommandManager();

        CommandManager(const CommandManager&) = delete;
        CommandManager& operator=(const CommandManager&) = delete;

        void recordCommandBuffers(VulkanContext& context, SwapChain& swapChain, RenderPassPipeline& pipeline,
                                  FrameBufferManager& framebufferManager, BufferManager& bufferManager);

        [[nodiscard]] const std::vector<VkCommandBuffer>& getCommandBuffers() const { return m_commandBuffers; }
        [[nodiscard]] VkCommandPool getCommandPool() const { return m_commandPool; }

    private:
        void createCommandPool(VulkanContext& context);

    private:
        VulkanContext* m_context{ nullptr };
        VkCommandPool m_commandPool{ VK_NULL_HANDLE };
        std::vector<VkCommandBuffer> m_commandBuffers;
    };

} // namespace vkp
