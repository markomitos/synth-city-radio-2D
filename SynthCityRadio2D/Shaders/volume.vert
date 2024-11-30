#version 330 core

layout(location = 0) in vec2 aPos; 
uniform float knobPosition;
uniform bool isKnob;
layout(location = 1) in vec3 inCol;
out vec3 chCol;
void main() {
    if (isKnob && aPos.x>-0.25f) {
        gl_Position = vec4(aPos.x + knobPosition, aPos.y, 0.0, 1.0); 
    }else{
    gl_Position = vec4(aPos, 0.0, 1.0); 
    }
    chCol = inCol;
    
}