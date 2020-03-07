#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 colorVert;

layout (binding = 0) coherent buffer block0
{
    vec2 h[];
};

layout (binding = 1) coherent buffer block1
{
    vec2 normalX[];
};

layout (binding = 2) coherent buffer block2
{
    vec2 normalZ[];
};

layout (binding = 3) coherent buffer block3
{
    vec2 dx[];
};

layout (binding = 4) coherent buffer block4
{
    vec2 dz[];
};

uniform float lambda;
uniform mat4 projection;
uniform mat4 view;

out vec4 colorFragIn;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec3 pos = position + vec3(-lambda*dx[gl_VertexID].x, h[gl_VertexID].x, -lambda*dz[gl_VertexID].x);
    gl_Position =  projection * view * vec4(pos.x, pos.y, pos.z, 1.0);
    FragPos = pos;
    colorFragIn = colorVert;
    Normal = vec3(normalX[gl_VertexID].x, 1.0f, normalZ[gl_VertexID].x);
    Normal = normalize(Normal);
}