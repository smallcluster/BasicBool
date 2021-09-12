#SHADER VERTEX
#version 330 core
layout(location = 0) in vec2 aPos;


uniform mat4 view;
uniform mat4 projection;

void main(){
    // TODO : change this to use a 2d Camera view
    gl_Position = projection * view * vec4(aPos, 0.0, 1.0);
}

#SHADER GEOMETRY
#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// size after view and projection scaling
uniform float width;
uniform float height;

void main() {
    vec4 pos = gl_in[0];

    // Bottom-left corner
    gl_Position = pos + vec4(0.0, -ProcessedWidth, 0.0, 0.0);
    EmitVertex();
    // Bottom-right corner
    gl_Position = pos + vec4(w, -ProcessedHeight, 0.0, 0.0);
    EmitVertex();
    // Top-left corner
    gl_Position = pos;
    EmitVertex();
    // Top-right corner
    gl_Position = pos + vec4(ProcessedHeight, 0.0, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}

#SHADER FRAGMENT
#version 330 core

out vec4 FragColor;

in vec4 fillColor;
in vec4 strokeColor;
int float strokeWeight;

vec4 drawOutline(){
    if(strokeWeight == 0 || strokeColor.a <= 0.0)
        return fillColor; 
}

vec4 drawInside() {
    if(fillColor.a <= 0.0)
        discard;
}

void main(){

    vec4 color = vec4(0.0);

    FragColor = fillColor;

}