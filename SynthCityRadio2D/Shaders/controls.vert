#version 330 core

layout(location = 0) in vec2 aPos; 
uniform float knobPosition;
uniform bool isKnob;

void main() {
    if (isKnob) {
        gl_Position = vec4(aPos.x + knobPosition, aPos.y, 0.0, 1.0); 
    }else{
    gl_Position = vec4(aPos, 0.0, 1.0); 
    }
    
}