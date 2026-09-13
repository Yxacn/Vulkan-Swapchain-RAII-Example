# CMakeLists.txt
# 顶层构建脚本：项目与版本声明、依赖查找（Vulkan/GLM/GLFW）、
# 着色器编译与版本头文件生成，末尾聚合 src/ 子目录。

cmake_minimum_required(VERSION 3.20)
project(VkProject VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 未指定构建类型时默认 Debug（保留调试符号与验证层，便于学习调试）；
# 需要最大性能时用 -DCMAKE_BUILD_TYPE=Release 配置
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(VKP_WARNINGS_AS_ERRORS "将编译警告视为错误" OFF)

# 版本与项目名由 project() 统一提供，生成头文件后工程内不再出现字面量副本
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/Version.hpp"
    @ONLY
)

find_package(Vulkan REQUIRED)
find_package(GLM REQUIRED)

# GLFW 随仓库提供静态库与头文件，无需单独安装
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib/libglfw3.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/glfw/include"
)

set(SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY "${SHADER_OUT_DIR}")

find_program(GLSLC_EXECUTABLE glslc REQUIRED)

function(compile_shader SHADER_SRC SHADER_SPV)
    add_custom_command(
        OUTPUT "${SHADER_SPV}"
        COMMAND "${GLSLC_EXECUTABLE}" -o "${SHADER_SPV}" "${SHADER_SRC}"
        DEPENDS "${SHADER_SRC}"
        COMMENT "Compiling shader ${SHADER_SRC}"
        VERBATIM
    )
endfunction()

compile_shader("${SHADER_SRC_DIR}/shader.vert" "${SHADER_OUT_DIR}/vert.spv")
compile_shader("${SHADER_SRC_DIR}/shader.frag" "${SHADER_OUT_DIR}/frag.spv")

add_custom_target(shaders DEPENDS "${SHADER_OUT_DIR}/vert.spv" "${SHADER_OUT_DIR}/frag.spv")

# 以编译宏注入核心库，渲染管线据此在运行期定位 .spv（末尾带分隔符）
file(TO_CMAKE_PATH "${SHADER_OUT_DIR}" SHADER_DIR_DEF)

# 统一编译选项：核心库与可执行文件共用，VKP_WARNINGS_AS_ERRORS 控制是否升级为错误
function(vkp_apply_compile_options TARGET)
    if(MSVC)
        # /utf-8：源码与执行字符集统一为 UTF-8，避免中文注释按本地代码页解析
        target_compile_options(${TARGET} PRIVATE /W4 /permissive- /utf-8)
        if(VKP_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET} PRIVATE /WX)
        endif()
    else()
        # C++20 指定初始化器省略的字段按标准零初始化，
        target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic
                                                  -Wno-missing-field-initializers)
        if(VKP_WARNINGS_AS_ERRORS)
            target_compile_options(${TARGET} PRIVATE -Werror)
        endif()
    endif()
endfunction()

add_subdirectory(src)
