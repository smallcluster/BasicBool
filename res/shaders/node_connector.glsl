#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 projection;
uniform mat4 transform;

out vec2 uv;

void main(){
    gl_Position = projection*transform*vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform float radius;
uniform float width;
uniform float height;

uniform vec3 insideColor;


float circle(vec2 p, vec2 c, float r){
    vec2 d = abs(p-c);
    return length(max(d, 0.0)) - r;
}


void main(){
    // fragment pos in the rectangle
    vec2 p = vec2(uv.x*width, uv.y*height);

    vec2 c = vec2(width/2.0, height/2.0);

    
    float d1 = circle(p, c, radius-1.5);
    float d2 = circle(p, c, 3.0*radius/4.0); // Inside
    float d = max(-d2, d1); // Ring

    // mixing coef
    float res = smoothstep(0., 1.5, d); // 1.5 px smoothing
    float res2 = step(0., d2);

    vec4 col = vec4(0);
    col = mix(vec4(insideColor, 1), col, res2);
    col = mix(vec4(1), col, res);
    
    // final result
    FragColor = col;
}

