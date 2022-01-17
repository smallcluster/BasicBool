#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in float aRadius;
layout (location = 3) in float aStatus;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out float radius;
out float status;

void main(){
    gl_Position = projection*view*vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
    radius = aRadius;
    status = aStatus;
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

in vec2 uv;
in float radius;
in float status;

const vec3 falseColor = vec3(1, 0, 0);
const vec3 trueColor = vec3(0, 1, 0);
const float smoothing = 4.0;


float circle(vec2 p, vec2 c, float r){
    vec2 d = abs(p-c);
    return length(max(d, 0.0)) - r;
}


void main(){
    // fragment pos in the rectangle
    vec2 p = vec2(uv.x*radius*2, uv.y*radius*2);
    vec2 c = vec2(radius, radius);
    float d1 = circle(p, c, radius);
    float d2 = circle(p, c, 3*(radius)/4); // Inside
    float d = max(-d2, d1); // Ring
    vec2 duv = fwidth(uv)*smoothing;
    float dd = length(duv*radius);
    float pixelDist1 = d * 2 / dd;
    float pixelDist2 = d2 * 2 / dd;
    float alphaRing = clamp(0.5 - pixelDist1, 0.0, 1.0);
    float alphaDisk = clamp(0.5 - pixelDist2, 0.0, 1.0);
    vec3 diskColor = (status == 1.0 ? trueColor : falseColor);
    vec3 ringColor = vec3(1);
    vec4 col = vec4(diskColor*alphaDisk+ringColor*alphaRing, alphaRing+alphaDisk);
    // final result
    FragColor = col;
}

