# src/CMakeLists.txt
# 核心库与可执行目标定义：引擎源码聚合为 vkp_core 静态库，
# VkProject 可执行文件只包含程序入口并链接核心库。

# 核心静态库：与入口点解耦，便于后续单元测试或复用到其他目标
add_library(vkp_core STATIC
    Application.cpp
    GLWindow.cpp
    VKEngine.cpp
    VulkanContext.cpp
    SwapChain.cpp
    RenderPassPipeline.cpp
    FrameBufferManager.cpp
    BufferManager.cpp
    CommandManager.cpp
    SyncManager.cpp
)
add_library(vkp::core ALIAS vkp_core)

# 可执行文件只包含程序入口，业务逻辑全部位于核心库
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE vkp::core)

# 统一关闭编译器扩展，核心库与可执行文件保持一致的 C++20 方言
set_target_properties(vkp_core ${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(vkp_core PUBLIC cxx_std_20)

# 头文件、版本头文件与第三方依赖都作为公共接口随链接传递给可执行文件
target_include_directories(vkp_core PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/generated"
)

# SHADER_DIR 由顶层脚本注入，RenderPassPipeline 据此定位运行期 .spv
target_compile_definitions(vkp_core PRIVATE SHADER_DIR="${SHADER_DIR_DEF}/")

target_link_libraries(vkp_core PUBLIC
    Vulkan::Vulkan
    glm::glm
    glfw
)

# 着色器是运行期资源，先于核心库构建，保证构建产物始终最新
add_dependencies(vkp_core shaders)

vkp_apply_compile_options(vkp_core)
vkp_apply_compile_options(${PROJECT_NAME})

# 可执行文件输出到构建根目录，保持与文档一致的运行路径
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if(NOT MSVC)
    # 静态链接运行库：摆脱对 MSYS2 DLL 的依赖，生成的 exe 可直接双击运行
    target_link_options(${PROJECT_NAME} PRIVATE -static)
endif()
