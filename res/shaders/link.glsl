#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aState;

uniform mat4 projection;
uniform mat4 transform;

out float state;

void main(){
    gl_Position = projection*transform*vec4(aPos.x, aPos.y, 0.0, 1.0);
    state = aState;
}

#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

in float state;

const vec3 trueColor = vec3(0, 1, 0);
const vec3 falseColor = vec3(1, 0, 0);

void main(){
    FragColor = vec4((state == 0.0 ? falseColor : trueColor), 1.0);
}

