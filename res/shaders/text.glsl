#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out vec3 color;

void main(){
    gl_Position = projection*view*vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
    color = aColor;
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

uniform sampler2D tex;

in vec2 uv;
in vec3 color;

void main(){
    FragColor = texture(tex, uv)*vec4(color, 1.0);
}

