#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform float time;
uniform bool isHorizontal; 
uniform mat4 projection;


void main() {
    vec3 position = aPos;

    if (isHorizontal) {
        
        position.z -= mod(time * 0.15, 1.0); 
        float offset = mod(time * 0.15, 1.0); 
        position.z = mod(aPos.z + 1.0 + offset, 2.0) - 1.0; 
    }
    
    gl_Position = projection * view * model * vec4(position, 1.0);
}