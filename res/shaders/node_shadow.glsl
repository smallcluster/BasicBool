#SHADER VERTEX
#version 330 core
layout (location = 0) in vec4 aRect;

void main(){
    gl_Position = aRect;
}

#SHADER GEOMETRY
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out vec2 size;

void main(){

    vec4 rect = gl_in[0].gl_Position;
    mat4 transfrom = projection*view;

    size = vec2(rect.z, rect.w);

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

uniform float smoothing;
uniform float radius;


float rect(vec2 p, vec2 c, float w, float h, float r){
    vec2 size = vec2(w-r, h-r);
    vec2 o = p-c;
    vec2 d = abs(o)-size;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;
}

void main(){
    float width = size.x;
    float height = size.y;

    // fragment pos in the rectangle
    vec2 p = vec2(uv.x*width, uv.y*height);

    // shape
    float d = rect(p, vec2(width/2.0, height/2.0), width/2.0-smoothing-1.5, height/2.0-smoothing-1.5, radius);

    // mixing coef
    float res = smoothstep(0., smoothing, d);

    // final result
    FragColor = mix(vec4(0, 0, 0, 0.25), vec4(0), res);
}

