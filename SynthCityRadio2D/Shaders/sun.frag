#version 330 core

in vec2 vPos; 

out vec4 FragColor;

uniform float time;

void main() {
    float t = (vPos.y + 1.0) / 2.0;

    vec3 topColor = vec3(1.0, 0.9, 0.0);  
    vec3 bottomColor = vec3(1.0, 0.0, 1.0); 

    vec3 finalColor = mix(bottomColor, topColor, t);

    
    finalColor *= 1.2; 
    float transparency = 1.0;

    if (vPos.y < 0.25){
        float stripePattern = sin(time * 3.5 + vPos.y * 100.0);
        transparency = smoothstep(0.1, 0.2, stripePattern);
    }

    FragColor = vec4(finalColor, transparency); 
}