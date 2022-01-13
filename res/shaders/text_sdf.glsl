#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aColor;

uniform mat4 projection;

out vec2 uv;
out vec3 color;

void main(){
    gl_Position = projection*vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
    color = aColor;
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

uniform sampler2D tex;

in vec2 uv;
in vec3 color;

const float width = 0.5;
const float sdfSize = 15;

// Adapted from : https://drewcassidy.me/2020/06/26/sdf-antialiasing/
void main(){
    vec4 col = vec4(color, 1);
    float dist = (width - texture(tex, uv).a)*sdfSize;
    vec2 duv = fwidth(uv);
    float dtex = length(duv*textureSize(tex, 0));
    float pixelDist = dist * 2 / dtex;
    col.a = clamp(0.5 - pixelDist, 0.0, 1.0);
    FragColor = col;
}

