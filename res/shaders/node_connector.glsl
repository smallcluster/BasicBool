#SHADER VERTEX
#version 330 core
layout (location = 0) in vec4 data; // (x,y,r,state)
layout (location = 1) in float aDepth;

out float vsdepth;

void main(){
    gl_Position = data;
    vsdepth = aDepth;
}

#SHADER GEOMETRY
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out float radius;
out float status;

in float vsdepth[];

void main(){

    vec4 data = gl_in[0].gl_Position;
    mat4 transfrom = projection*view;
    float depth = vsdepth[0];

    radius = data.z;
    status = data.w;

    // p1
    gl_Position = transfrom*vec4(data.x-radius, data.y+radius, depth, 1);
    uv = vec2(0,1);
    EmitVertex();

    // p2
    gl_Position = transfrom*vec4(data.x+radius, data.y+radius, depth, 1);
    uv = vec2(1,1);
    EmitVertex();

    // p3
    gl_Position = transfrom*vec4(data.x-radius, data.y-radius, depth, 1);
    uv = vec2(0,0);
    EmitVertex();

    // p4
    gl_Position = transfrom*vec4(data.x+radius, data.y-radius, depth, 1);
    uv = vec2(1,0);
    EmitVertex();

    EndPrimitive();
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

in vec2 uv;
in float radius;
in float status;

uniform vec3 falseColor;
uniform vec3 trueColor;

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

