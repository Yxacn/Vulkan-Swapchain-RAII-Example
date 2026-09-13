// VkCheck.hpp
// Vulkan 返回值检查：把非 VK_SUCCESS 的结果统一转成携带 VkResult 数值的异常，
// 调用点只需处理成功路径，错误信息不会因遗漏分支而丢失。
#pragma once

#include <stdexcept>
#include <string>

#include <vulkan/vulkan.h>

namespace vkp
{
    // 统一 Vulkan 返回码检查：失败时抛出带错误码的异常，避免各调用点遗漏处理
    inline void checkVk(VkResult result, const char* message)
    {
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(std::string(message) + " (VkResult " + std::to_string(static_cast<int>(result)) +
                                     ")");
        }
    }
} // namespace vkp
