// shader.frag
// 片段着色器：把插值后的顶点颜色直接作为最终输出（location 0），不做额外光照。
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}