R"(

#SHADER VERTEX
#version 330 core
layout(location = 0) in vec2 aPos;

void main(){
    // TODO : change this to use a 2d Camera view
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#SHADER GEOMETRY
#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// size after view and projection scaling
uniform float width;
uniform float height;
varying vec2 uv;

void main() {
    vec4 pos = gl_in[0].gl_Position;

    // Bottom-left corner
    gl_Position = pos + vec4(0.0, -height, 0.0, 0.0);
    uv = vec2(0.0, 1.0);
    EmitVertex();
    // Bottom-right corner
    gl_Position = pos + vec4(width, -height, 0.0, 0.0);
    uv = vec2(1.0, 1.0);
    EmitVertex();
    // Top-left corner
    gl_Position = pos;
    uv = vec2(0.0, 0.0);
    EmitVertex();
    // Top-right corner
    gl_Position = pos + vec4(width, 0.0, 0.0, 0.0);
    uv = vec2(1.0, 0.0);
    EmitVertex();

    EndPrimitive();
}

#SHADER FRAGMENT
#version 330 core

out vec4 FragColor;
uniform vec4 fillColor;
uniform vec4 strokeColor;
uniform vec2 sw;
varying vec2 uv;

void main(){

    if((uv.x <= sw.x || uv.x >= 1.0-sw.x || uv.y <= sw.y || uv.y >= 1.0-sw.y) && strokeColor.a > 0.0)
        FragColor = strokeColor;
    else if(fillColor.a > 0.0)
        FragColor = fillColor;
    else
        discard;
}

)"