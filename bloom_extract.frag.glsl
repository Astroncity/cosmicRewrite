#version 330
precision mediump float;

uniform sampler2D texture0;
varying vec2 fragTexCoord;

void main() {
    vec4 color = texture2D(texture0, fragTexCoord);
    float brightness = dot(color.rgb, vec3(0.299, 0.587, 0.114)); // Use NTSC brightness
    if (brightness > 1.0) {
        gl_FragColor = color; // Keep bright colors
    } else {
        gl_FragColor = vec4(0.0); // Discard non-bright colors
    }
}
