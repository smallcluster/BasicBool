#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec2 aSize;
layout (location = 3) in float aHeaderHeight;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out vec2 size;
out float headerHeight;

void main(){
    gl_Position = projection*view*vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
    size = aSize;
    headerHeight = aHeaderHeight;
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec2 size;
in float headerHeight;

uniform float radius;

const vec3 headerColor = vec3(0, 0, 1);
const vec3 bodyColor = vec3(0.2);
const float smoothing = 4;


// SDF functions
float rect(vec2 p, vec2 c, float w, float h, float r){
    vec2 size = vec2(w-r, h-r);
    vec2 o = p-c;
    vec2 d = abs(o)-size;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;
}

void main(){
    float width = size.x;
    float height = size.y;
    vec2 p = vec2(uv.x*width, uv.y*height);
    float d = rect(p, vec2(width/2.0, height/2.0), width/2.0-1.5, height/2.0-1.5, radius);
    vec2 duv = fwidth(uv)*smoothing;
    float dd = length(duv*size);
    float pixelDist = d * 2 / dd;
    float alpha = clamp(0.5 - pixelDist, 0.0, 1.0);
    float header = 1-step(headerHeight, p.y);
    vec4 col = vec4(headerColor*header+bodyColor*(1-header), alpha);
    FragColor = col;
}

