#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aState;

out float vsState;

void main(){
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vsState = aState;
}

#SHADER GEOMETRY
#version 330 core
#define RES 32
#define VERTICES 64
layout (lines) in;
layout (triangle_strip, max_vertices = VERTICES) out; 

uniform mat4 projection;
uniform mat4 view;
uniform float width;

out vec2 uv;
in float vsState[];
out float state;


const float step = 1.0/(RES-1);

vec2 bezier(float t, vec2 start, vec2 end, vec2 control1, vec2 control2){
    return (1.0-t)*(1.0-t)*(1.0-t)*start + 3*(1.0-t)*(1.0-t)*t*control1 + 3*(1.0-t)*t*t*control2 + t*t*t*end;
}

void main(){
    float radius = width/2.0;
    vec2 start = vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y);
    vec2 end = vec2(gl_in[1].gl_Position.x, gl_in[1].gl_Position.y);

    vec2 dir = end-start;
    vec2 control1 = start + vec2(abs(dir.x)/2.0, 0);
    vec2 control2 = end - vec2(abs(dir.x)/2.0, 0);

    vec2 ortho = normalize(vec2(0, abs(dir.x)));

    mat4 transfrom = projection*view;

    // first vert
    gl_Position = transfrom*vec4(start + ortho*radius, 0, 1);
    uv = vec2(0,0);
    state = vsState[0];
    EmitVertex();
    gl_Position = transfrom*vec4(start - ortho*radius, 0, 1);
    uv = vec2(0,1);
    state = vsState[0];
    EmitVertex();
    
    // inside vert
    for(int i=0; i < RES-2; i++){
        vec2 p1 = bezier(i*step, start, end, control1, control2);
        vec2 p2 = bezier((i+1)*step, start, end, control1, control2);
        vec2 p3 = bezier((i+2)*step, start, end, control1, control2);

        vec2 d1 = p2-p1;
        vec2 d2 = p3-p2;
        vec2 o1 = normalize(vec2(-d1.y, d1.x));
        vec2 o2 = normalize(vec2(-d2.y, d2.x));
        vec2 v = normalize(o1+o2);

        gl_Position = transfrom*vec4(p2 + v*radius, 0, 1);
        uv = vec2(i+1, 0);
        state = vsState[0];
        EmitVertex();
        gl_Position = transfrom*vec4(p2 - v*radius, 0, 1);
        uv = vec2(i+1, 1);
        state = vsState[0];
        EmitVertex();
    }
    
    // last vert
    gl_Position = transfrom*vec4(end + ortho*radius, 0, 1);
    uv = vec2(RES-1,0);
    state = vsState[0];
    EmitVertex();
    gl_Position = transfrom*vec4(end - ortho*radius, 0, 1);
    uv = vec2(RES-1,1);
    state = vsState[0];
    EmitVertex();

    EndPrimitive();
}

#SHADER FRAGMENT
#version 330 core

out vec4 FragColor;
in vec2 uv;
in float state;

const vec3 trueColor = vec3(0, 1, 0);
const vec3 falseColor = vec3(1, 0, 0);

uniform float width;

const float smooting = 0.5;

void main(){

    float d = (width/2.0-smooting-abs(uv.y*width-width/2.0));
    float dw = fwidth(d);

    float pixelDist = d / dw;
    float alpha = 1-clamp(0.5 - pixelDist, 0.0, 1.0);


    FragColor = vec4((state == 0.0 ? falseColor : trueColor), alpha);
}
