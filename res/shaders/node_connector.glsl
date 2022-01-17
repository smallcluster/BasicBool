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


float circle(vec2 p, vec2 c, float r){
    vec2 d = abs(p-c);
    return length(max(d, 0.0)) - r;
}


void main(){
    // fragment pos in the rectangle
    vec2 p = vec2(uv.x*radius*2, uv.y*radius*2);

    vec2 c = vec2(radius, radius);

    
    float d1 = circle(p, c, radius-1.5);
    float d2 = circle(p, c, 3.2*radius/4.0); // Inside
    float d = max(-d2, d1); // Ring

    // mixing coef
    // TODO : use fwidth
    float res = smoothstep(0., 1.5, d); // 1.5 px smoothing
    float res2 = step(0., d2);

    vec4 col = vec4(0);
    col = mix(vec4((status == 1.0 ? trueColor : falseColor), 1), col, res2);
    col = mix(vec4(1), col, res);
    
    // final result
    FragColor = col;
}

