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

void main() {
    vec4 pos = gl_in[0].gl_Position;

    // Bottom-left corner
    gl_Position = pos + vec4(0.0, -height, 0.0, 0.0);
    EmitVertex();
    // Bottom-right corner
    gl_Position = pos + vec4(width, -height, 0.0, 0.0);
    EmitVertex();
    // Top-left corner
    gl_Position = pos;
    EmitVertex();
    // Top-right corner
    gl_Position = pos + vec4(width, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}

#SHADER FRAGMENT
#version 330 core

out vec4 FragColor;

uniform vec4 fillColor;

void main(){
    // TODO : handle colors & borders
    FragColor = fillColor;
}

)"