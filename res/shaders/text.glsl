#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 projection;

out vec2 uv;

void main(){
    gl_Position = projection*vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

uniform sampler2D tex;

uniform vec3 color;

in vec2 uv;

void main(){
    FragColor = texture(tex, uv)*vec4(color, 1.0);
}

