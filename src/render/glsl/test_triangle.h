R"(

#SHADER VERTEX
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

out vec3 color;

void main(){
    gl_Position = vec4(aPos, 0.0, 1.0); 
    color = aColor;
}

#SHADER FRAGMENT
#version 330 core

out vec4 FragColor;
in vec3 color;

void main(){
    FragColor = vec4(color, 1.0);
}

)"