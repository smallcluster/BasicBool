#SHADER VERTEX
#version 330 core
layout (location = 0) in vec4 aRect;

void main(){
    gl_Position = aRect;
}

#SHADER GEOMETRY
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

uniform mat4 projection;
uniform mat4 view;

void main(){
    vec4 rect = gl_in[0].gl_Position;
    mat4 transfrom = projection*view;
    // p1
    gl_Position = transfrom*vec4(rect.x, rect.y+rect.w, 0, 1);
    EmitVertex();
    // p2
    gl_Position = transfrom*vec4(rect.x+rect.z, rect.y+rect.w, 0, 1);
    EmitVertex();
    // p3
    gl_Position = transfrom*vec4(rect.x, rect.y, 0, 1);
    EmitVertex();
    // p4
    gl_Position = transfrom*vec4(rect.x+rect.z, rect.y, 0, 1);
    EmitVertex();
    EndPrimitive();
}


#SHADER FRAGMENT
#version 330 core
out vec4 FragColor;

uniform vec3 selectedColor;

void main(){
    FragColor = vec4(selectedColor, 0.25);
}

