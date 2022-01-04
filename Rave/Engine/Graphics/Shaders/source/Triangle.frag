#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
	vec4 color;
} ubo;

void main() {
    outColor = ubo.color;
}