#SHADER VERTEX
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 pmat;

out vec2 uv;

void main(){
    gl_Position = pmat*vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform vec4 color;

void main(){

    // TODO : use 2d sdf to draw a rounded box shadow
    FragColor = mix(color, vec4(uv, 0.0, 1.0), 1.0-smoothstep(0.5-0.01, 0.5, distance(uv, vec2(0.5, 0.5))));
}

