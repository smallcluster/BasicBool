#SHADER VERTEX
#version 330 core
layout (location = 0) in vec4 aRect;   // (x,y,w,h) in world space
layout (location = 1) in vec4 aRectUV; // (x,y,w,h) in [0,1]
layout (location = 2) in vec3 aColor;

out vec4 rectUV;
out vec3 vscolor;

void main(){
    gl_Position = aRect; // (x,y,w,h)
    rectUV = aRectUV;
    vscolor = aColor;
}


#SHADER GEOMETRY
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

uniform mat4 projection;
uniform mat4 view;

in vec3 vscolor[];
in vec4 rectUV[];

out vec2 uv;
out vec3 color;

void main(){
    vec4 rect = gl_in[0].gl_Position;
    mat4 transfrom = projection*view;

    color = vscolor[0];

    // p1
    gl_Position = transfrom*vec4(rect.x, rect.y+rect.w, 0, 1);
    uv = vec2(rectUV[0].x, rectUV[0].y+rectUV[0].w);
    EmitVertex();

    // p2
    gl_Position = transfrom*vec4(rect.x+rect.z, rect.y+rect.w, 0, 1);
    uv = vec2(rectUV[0].x+rectUV[0].z, rectUV[0].y+rectUV[0].w);
    EmitVertex();

    // p3
    gl_Position = transfrom*vec4(rect.x, rect.y, 0, 1);
    uv = rectUV[0].xy;
    EmitVertex();

    // p4
    gl_Position = transfrom*vec4(rect.x+rect.z, rect.y, 0, 1);
    uv = vec2(rectUV[0].x+rectUV[0].z, rectUV[0].y);
    EmitVertex();
    

    EndPrimitive();
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

uniform sampler2D tex;

in vec2 uv;
in vec3 color;

const float width = 0.5;
const float sdfSize = 20;

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

