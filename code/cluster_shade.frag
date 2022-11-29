#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const vec4 shadeColors[6] = {
    //{92.0f/256, 104.0f/256, 235.0f/256, 1.0f}, 
    //{86.0f/256, 79.0f/256, 50.0f/256, 1.0f}, 
    //{235.0f/256, 218.0f/256, 64.0f/256, 1.0f}, 
    //{73.0f/256, 195.0f/256, 234.0f/256, 1.0f}, 
    //{201.0f/256, 177.0f/256, 205.0f/256, 1.0f},
    //{235.0f/256, 218.0f/256, 64.0f/256, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f, 1.0f},
};

void main() {
    //outColor = texture(texSampler, fragTexCoord);
    outColor = shadeColors[(gl_PrimitiveID / 124) % 6];
}
