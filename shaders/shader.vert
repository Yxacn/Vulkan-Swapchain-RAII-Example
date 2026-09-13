// shader.vert
// 顶点着色器：以 UBO（std140 布局，binding 0）中的 model/view/proj 变换顶点位置，
// 顶点颜色由 location 1 输入、location 0 输出，供片段着色器插值。
#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}