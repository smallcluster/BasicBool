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
    float d = sqrt((uv.x-0.5)*(uv.x-0.5)+(uv.y-0.5)*(uv.y-0.5));
    float delta = 0.01;
    float alpha = 1.0 - smoothstep(0.45 - delta, 0.45, d);
    float stroke = 1.0 - smoothstep(0.5-sw.x*2 - delta, 0.5-sw.x*2 + delta, d);
    FragColor = vec4(mix(strokeColor.rgb, fillColor.rgb, stroke), alpha);
}

)"