#version 330 core
layout(location = 0) in vec2 aPos;

uniform float time;
uniform bool isHorizontal; 

void main() {
    vec2 position = aPos;

    if (isHorizontal) {
        
        position.y += mod(time * 0.15, 1.0); 
        if (position.y > 0) {
            position.y = position.y - 1; 
        }
    }
    
    gl_Position = vec4(position, 0.0, 1.0);
}
