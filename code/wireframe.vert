#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

const mat4 ScaleMat = {
    {1.1f, 0.0f, 0.0f, 0.0f}, 
    {0.0f, 1.1f, 0.0f, 0.0f}, 
    {0.0f, 0.0f, 1.1f, 0.0f}, 
    {0.0f, 0.0f, 0.0f, 0.0f}, 
};
void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * ScaleMat * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
