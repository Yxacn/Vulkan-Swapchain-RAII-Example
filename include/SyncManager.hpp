// SyncManager.hpp
// 同步管理：逐帧创建 imageAvailable 信号量与围栏，逐图像创建 renderFinished 信号量。
// 在飞帧数默认 2 且不超过交换链图像数；围栏创建时即为已触发状态，首帧无需特殊处理。
#pragma once

#include <array>
#include <cassert>
#include <vector>

#include <vulkan/vulkan.h>

namespace vkp
{
    class VulkanContext;

    inline constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    class SyncManager
    {
    public:
        SyncManager(VulkanContext& context, uint32_t imageCount, uint32_t frameCount = MAX_FRAMES_IN_FLIGHT);
        ~SyncManager();

        SyncManager(const SyncManager&) = delete;
        SyncManager& operator=(const SyncManager&) = delete;

        [[nodiscard]] uint32_t getFrameCount() const { return m_frameCount; }
        // 仅前 m_frameCount 项已创建，调用方应按 getFrameCount() 限制索引范围
        [[nodiscard]] const std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT>& getImageAvailableSemaphores() const
        {
            return m_imageAvailableSemaphores;
        }
        [[nodiscard]] const std::vector<VkSemaphore>& getRenderFinishedSemaphores() const
        {
            return m_renderFinishedSemaphores;
        }
        // 与 getImageAvailableSemaphores 相同：仅前 m_frameCount 项已创建
        [[nodiscard]] const std::array<VkFence, MAX_FRAMES_IN_FLIGHT>& getInFlightFences() const
        {
            return m_inFlightFences;
        }
        [[nodiscard]] VkFence getImageInFlight(uint32_t imageIndex) const
        {
            assert(imageIndex < m_imagesInFlight.size());
            return m_imagesInFlight[imageIndex];
        }
        void setImageInFlight(uint32_t imageIndex, VkFence fence)
        {
            assert(imageIndex < m_imagesInFlight.size());
            m_imagesInFlight[imageIndex] = fence;
        }

    private:
        void createSyncObjects(VulkanContext& context, uint32_t imageCount);
        void destroySyncObjects() noexcept;

    private:
        VulkanContext* m_context{ nullptr };
        uint32_t m_frameCount{ MAX_FRAMES_IN_FLIGHT };
        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_imageAvailableSemaphores{};
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_inFlightFences{};
        std::vector<VkFence> m_imagesInFlight;
    };

} // namespace vkp
