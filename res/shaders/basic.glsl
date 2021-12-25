#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 pmat;

void main(){
    gl_Position = pmat*vec4(aPos.x, aPos.y, 0.0, 1.0);
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

uniform vec3 color;

void main(){
    FragColor = vec4(color, 1.0);
}

