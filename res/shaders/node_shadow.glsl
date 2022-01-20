#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec2 aSize;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out vec2 size;

void main(){
    gl_Position = projection*view*vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
    size = aSize;
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

