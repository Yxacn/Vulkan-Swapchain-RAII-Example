// FrameBufferManager.cpp
// 帧缓冲管理实现：逐图像视图创建帧缓冲，任一失败即回收已创建对象后抛出，
// 避免构造函数抛异常时遗留部分句柄。
#include "FrameBufferManager.hpp"

#include <cassert>
#include <stdexcept>

#include "RenderPassPipeline.hpp"
#include "SwapChain.hpp"
#include "VulkanContext.hpp"

namespace vkp
{
    FrameBufferManager::FrameBufferManager(VulkanContext& context, SwapChain& swapChain, RenderPassPipeline& pipeline)
        : m_context(&context)
    {
        createFramebuffers(context, swapChain, pipeline.getRenderPass());
    }

    FrameBufferManager::~FrameBufferManager()
    {
        if (m_context)
            destroyFramebuffers(*m_context);
    }

    void FrameBufferManager::destroyFramebuffers(VulkanContext& context) noexcept
    {
        for (VkFramebuffer framebuffer : m_swapChainFramebuffers)
        {
            if (framebuffer)
                vkDestroyFramebuffer(context.getDevice(), framebuffer, nullptr);
        }
        m_swapChainFramebuffers.clear();
    }

    void FrameBufferManager::createFramebuffers(VulkanContext& context, SwapChain& swapChain, VkRenderPass renderPass)
    {
        const auto& imageViews = swapChain.getImageViews();
        assert(!imageViews.empty());

        m_swapChainFramebuffers.assign(imageViews.size(), VK_NULL_HANDLE);

        for (size_t i = 0; i < imageViews.size(); ++i)
        {
            VkImageView attachments[] = { imageViews[i] };
            const VkFramebufferCreateInfo framebufferInfo{
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = renderPass,
                .attachmentCount = 1,
                .pAttachments = attachments,
                .width = swapChain.getExtent().width,
                .height = swapChain.getExtent().height,
                .layers = 1,
            };

            if (vkCreateFramebuffer(context.getDevice(), &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) !=
                VK_SUCCESS)
            {
                destroyFramebuffers(context);
                throw std::runtime_error("Failed to create framebuffer!");
            }
        }
    }

} // namespace vkp
