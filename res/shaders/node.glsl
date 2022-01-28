#SHADER VERTEX
#version 330 core
layout (location = 0) in vec4 aRect;
layout (location = 1) in float aHeaderHeight;
layout (location = 2) in float aSelected;

out float vsSelected;
out float vsHeaderHeight;

void main(){
    gl_Position = aRect;
    vsHeaderHeight = aHeaderHeight;
    vsSelected = aSelected;
}

#SHADER GEOMETRY
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

uniform mat4 projection;
uniform mat4 view;

in float vsHeaderHeight[];
in float vsSelected[];

out vec2 uv;
out vec2 size;
out float headerHeight;
out float selected;

void main(){
    selected = vsSelected[0];
    vec4 rect = gl_in[0].gl_Position;
    mat4 transfrom = projection*view;

    size = vec2(rect.z, rect.w);
    headerHeight = vsHeaderHeight[0];

    // p1
    gl_Position = transfrom*vec4(rect.x, rect.y+rect.w, 0, 1);
    uv = vec2(0,1);
    EmitVertex();

    // p2
    gl_Position = transfrom*vec4(rect.x+rect.z, rect.y+rect.w, 0, 1);
    uv = vec2(1,1);
    EmitVertex();

    // p3
    gl_Position = transfrom*vec4(rect.x, rect.y, 0, 1);
    uv = vec2(0,0);
    EmitVertex();

    // p4
    gl_Position = transfrom*vec4(rect.x+rect.z, rect.y, 0, 1);
    uv = vec2(1,0);
    EmitVertex();

    EndPrimitive();
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec2 size;
in float headerHeight;
in float selected;

uniform float radius;
uniform vec3 headerColor;
uniform vec3 selectedColor;
uniform vec3 bodyColor;
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
    vec4 col = vec4( (selected == 1.0 ? selectedColor : headerColor)*header+bodyColor*(1-header), alpha);
    FragColor = col;
}

