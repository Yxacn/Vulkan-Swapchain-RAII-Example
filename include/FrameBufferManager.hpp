// FrameBufferManager.hpp
// 帧缓冲管理：为每张交换链图像视图创建一个帧缓冲并持有句柄。
// 帧缓冲引用渲染通道，销毁必须先于 RenderPassPipeline（见 VKEngine 的清理顺序）。
#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace vkp
{
    class VulkanContext;
    class SwapChain;
    class RenderPassPipeline;

    class FrameBufferManager
    {
    public:
        FrameBufferManager(VulkanContext& context, SwapChain& swapChain, RenderPassPipeline& pipeline);
        ~FrameBufferManager();

        FrameBufferManager(const FrameBufferManager&) = delete;
        FrameBufferManager& operator=(const FrameBufferManager&) = delete;

        [[nodiscard]] const std::vector<VkFramebuffer>& getFramebuffers() const { return m_swapChainFramebuffers; }

    private:
        void destroyFramebuffers() noexcept;
        void createFramebuffers(VulkanContext& context, SwapChain& swapChain, VkRenderPass renderPass);

    private:
        VulkanContext* m_context{ nullptr };
        std::vector<VkFramebuffer> m_swapChainFramebuffers;
    };

} // namespace vkp
